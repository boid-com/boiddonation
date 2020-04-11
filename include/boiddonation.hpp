#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <cmath>
#define DAY_MS 8.64e7
// #define DAY_MS 3.6e6
// #define TOKEN_CONTRACT "token.boid"
#define TOKEN_CONTRACT "boidcomtoken"
using namespace eosio;
using eosio::microseconds;

class[[eosio::contract("boiddonation")]] boiddonation : public eosio::contract
{
private:
  TABLE contributors
  {
    eosio::name account;
    std::vector <eosio::asset> coins;
    uint32_t donations = 0;
    uint32_t level = 1;
    uint32_t total_power_granted = 0;
    uint64_t primary_key() const { return account.value; }
  };

  using contributors_table = eosio::multi_index<"contributors"_n, contributors>;

  TABLE tokens
  {
    extended_symbol token;
    float min_contribution = 0;
    float total_contributed = 0;
    uint32_t unique_contributors = 0;
    uint32_t daily_power_regen = 1000;
    float current_power_available = 0;
    uint32_t power_available_cap = 1000;
    uint64_t power_last_updated_utc_ms = 0;
    uint8_t power_reward_multiplier = 1;
    uint32_t total_power_granted = 0;
    uint64_t primary_key() const { return token.get_symbol().raw(); }
  };
  using tokens_table = eosio::multi_index<"tokens"_n, tokens>;
  
  template <typename T>
  void cleanTable(name code, uint64_t account, const uint32_t batchSize) {
    T db(code, account);
    uint32_t counter = 0;
    auto itr = db.begin();
    while (itr != db.end() && counter++ < batchSize) {
      itr = db.erase(itr);
    }
  }

  microseconds now();
  auto get_config();

  uint64_t curr_time_ms = eosio::current_time_point().time_since_epoch().count()/1000;

  float generate_token_power(uint32_t daily_power_regen, uint64_t pwr_last_updated, uint32_t power_available_cap, float current_power_available) {
    if (pwr_last_updated == 0) pwr_last_updated = curr_time_ms;
    return fmax(fmin(power_available_cap,current_power_available + (float)daily_power_regen/DAY_MS * (curr_time_ms - pwr_last_updated)),0);
  }
  uint8_t find_power_reward_multiplier(uint32_t updated_tkn_pwr, uint8_t user_power_reward_multiplier_max,uint32_t power_available_cap){
    return (((updated_tkn_pwr - 0) * ((user_power_reward_multiplier_max + 1) - 1)) / ((power_available_cap + 1) - 0)) + 1;
  }
  float round(float var) 
  { 
      float value = (int)(var * 100 + .5); 
      return (float)value / 100; 
  } 

public:
  using contract::contract;

  struct token
  {
    extended_symbol token;
    uint32_t quantity;
    uint32_t daily_power_regen;
    uint32_t power_available_cap;
    float current_power_available;

  };

  TABLE config
  {
    bool admin_pause = bool(false);
    uint32_t user_power_reward_cap = 100;
    uint8_t user_power_reward_multiplier_max = 5;
    uint8_t user_power_reward_increment = 10;
    uint8_t user_donations_per_level = 10;
    uint8_t user_level_cap = 10;
    uint64_t promotion_start_utc_ms = 1585179136049;
    uint64_t promotion_end_utc_ms = 1685199897017;
    uint8_t min_contribution_percent_increase = 1;
  };
  typedef eosio::singleton<"config"_n, config> config_table;

  [[eosio::on_notify("*::transfer")]] void deposit(name from, name to, eosio::asset quantity, std::string memo);

  ACTION setconfig(config conf);
  ACTION initconfig();
  ACTION clearconfig();
  ACTION addtokens(std::vector <token> tokens_v);
  ACTION cleartokens();
  ACTION clearcontrib(uint32_t rows);
  ACTION gentknpwr();
  ACTION erasetoken(symbol token_symbol);
};