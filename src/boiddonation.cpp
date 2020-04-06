#include <boiddonation.hpp>
#include <functions.cpp>


void boiddonation::deposit(name from, name to, eosio::asset quantity, std::string memo) {
  if (from == get_self() || to != get_self()) return;
  
  auto conf = get_config();

  check(conf.admin_pause == false, "Deposits are paused.");
  check(conf.promotion_end_utc_ms > curr_time_ms, "Promotion has ended.");
  check(conf.promotion_start_utc_ms < curr_time_ms, "Promotion hasn't started yet.");

  uint64_t elapsed_since_promo_start_ms = curr_time_ms - conf.promotion_start_utc_ms;
  uint64_t day_ms = hours(24).count()/1000;
  
  tokens_table tokens_t(get_self(),get_self().value);
  auto tkn_row = tokens_t.end();
  for (auto token_itr = tokens_t.begin(); token_itr != tokens_t.end(); token_itr++) {
    if (get_first_receiver() == token_itr->token.get_contract() && quantity.symbol == token_itr->token.get_symbol()) 
    {
      tkn_row = token_itr;
      break;
    }
  }

  if (tkn_row == tokens_t.end()) {print("no valid token found"); return;}
  float precision_coef = pow(10, quantity.symbol.precision());
  uint64_t min_cont = uint64_t(tkn_row->min_contribution*1000);
  uint64_t cont = uint64_t((quantity.amount/precision_coef)*1000);
  print("min_cont: ",min_cont,"\n");
  print("cont: ",cont,"\n");

  check(min_cont <= cont,"The minimum donation for this token is: "+std::to_string(min_cont)+" You tried to send: "+std::to_string(cont));

  contributors_table contributors_t(get_self(),get_self().value);
  auto contributor_itr = contributors_t.find(from.value);
  auto acct_level = 0;
  if (contributor_itr == contributors_t.end()){
    contributors_t.emplace(get_self(), [&](auto &row) {
      row.account = from;
      row.total_power_granted = 5;
      row.coins.push_back(quantity);
      row.donations = 1;
      row.level = 0;
      acct_level = row.level;
      tokens_t.modify(tkn_row,get_self(),[&](auto &row){
        row.unique_contributors ++;
      });
    });
  } else {
    contributors_t.modify(contributor_itr, get_self(), [&](auto &row) {
      row.donations += 1;
      row.level = fmin(conf.user_level_cap , row.donations / conf.user_donations_per_level);
      acct_level = row.level;
      bool need_insert = false;
      for (auto coins_itr = row.coins.begin() ; coins_itr != row.coins.end(); ++coins_itr) {
        if (coins_itr->symbol == quantity.symbol) {coins_itr->set_amount(coins_itr->amount + quantity.amount); break;}
        else if (coins_itr + 1 == row.coins.end()) need_insert = true;
      }
      if (need_insert) {
        row.coins.push_back(quantity);
        tokens_t.modify(tkn_row,get_self(),[&](auto &row){
          row.unique_contributors ++;
        });
        }
    });
  }

  uint32_t updated_tkn_pwr = generate_token_power(tkn_row->daily_power_regen,tkn_row->power_last_updated_utc_ms,tkn_row->power_available_cap,tkn_row->current_power_available);
  print("updated_tkn_pwr",updated_tkn_pwr,"\n");
  uint8_t power_reward_multiplier = find_power_reward_multiplier(updated_tkn_pwr,conf.user_power_reward_multiplier_max,tkn_row->power_available_cap);
  print("Power Bias: ",power_reward_multiplier,"\n");
  
  float boidpower = 0;
  if (acct_level < 1) boidpower = fmin(5,updated_tkn_pwr);
  else boidpower = fmax(fmin(updated_tkn_pwr, fmin((acct_level) * conf.user_power_reward_increment ,conf.user_power_reward_cap) * power_reward_multiplier),0);
  
  if (contributor_itr != contributors_t.end()) {
    contributors_t.modify(contributor_itr, get_self(), [&](auto &row) { 
      row.total_power_granted += (uint32_t)boidpower;
    });
  }

  float current_power_available = (updated_tkn_pwr - boidpower);
  print("current_power_available",current_power_available,"\n");
  // check(current_power_available > 0,"Error calculating token power remaining.");

  tokens_t.modify(tkn_row,get_self(),[&](auto &row){
    row.total_contributed += quantity.amount / precision_coef;
    const float min_increase = row.min_contribution * ((float)conf.min_contribution_percent_increase/100);
    const float max_increase = fmax((quantity.amount / precision_coef) * ((float)conf.min_contribution_percent_increase/100),0.1);
    const float new_min_contribution = fmin(max_increase,min_increase);
    print("min_increase: ",min_increase,"\n");
    print("max_increase: ",max_increase,"\n");
    print("new min_contribution: ",new_min_contribution,"\n");
    row.min_contribution += new_min_contribution;
    row.current_power_available = current_power_available;
    row.power_last_updated_utc_ms = curr_time_ms;
    row.power_reward_multiplier = power_reward_multiplier;
    row.total_power_granted += (uint32_t)boidpower;
  });

   if (boidpower > 0) {
     eosio::action(permission_level{name(TOKEN_CONTRACT), "active"_n}, name(TOKEN_CONTRACT), "updatepower"_n, std::make_tuple(from,boidpower)).send();
   }
}

ACTION boiddonation::setconfig(config conf){
    require_auth(get_self() );
    config_table _config(get_self(), get_self().value);
    _config.set(conf, get_self());
}
ACTION boiddonation::initconfig(){get_config();}

ACTION boiddonation::clearconfig(){
  require_auth(get_self());
  config_table _config(get_self(), get_self().value);
  _config.remove();
}
ACTION boiddonation::gentknpwr(){
  tokens_table tkns(get_self(),get_self().value);
  auto conf = get_config();
  for(auto tkns_itr = tkns.begin(); tkns_itr != tkns.end(); tkns_itr++) {
      if (tkns_itr->power_last_updated_utc_ms > curr_time_ms - 30000) continue;
      tkns.modify(tkns_itr,get_self(),[&](auto &row){
        auto updated_tkn_pwr = generate_token_power(row.daily_power_regen,row.power_last_updated_utc_ms,row.power_available_cap,row.current_power_available);
        row.current_power_available = updated_tkn_pwr;
        row.power_last_updated_utc_ms = curr_time_ms;
        row.power_reward_multiplier = find_power_reward_multiplier(updated_tkn_pwr,conf.user_power_reward_multiplier_max,row.power_available_cap);
      });
  }
}
ACTION boiddonation::addtokens(std::vector <token> tokens_v){
  require_auth(get_self());
  tokens_table tkns(get_self(),get_self().value);
  for(int i = 0; i < tokens_v.size(); i++) {
    tkns.emplace(get_self(), [&](auto &row) {
      row.token = tokens_v[i].token;
      row.current_power_available = tokens_v[i].current_power_available;
      row.min_contribution = tokens_v[i].quantity;
      row.daily_power_regen = tokens_v[i].daily_power_regen;
      row.power_available_cap = tokens_v[i].power_available_cap;
      row.power_last_updated_utc_ms = curr_time_ms;
    });
  }
}
ACTION boiddonation::cleartokens(){
  require_auth(get_self());
  tokens_table tkns(get_self(),get_self().value);
  for(auto tkns_itr = tkns.begin(); tkns_itr != tkns.end();) {
      tkns_itr = tkns.erase(tkns_itr);
  }
}
ACTION boiddonation::clearcontrib(uint32_t rows){
  require_auth(get_self());
  cleanTable<contributors_table>(get_self(), get_self().value, rows);
}