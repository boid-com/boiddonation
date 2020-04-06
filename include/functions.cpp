microseconds boiddonation::now() {
  return microseconds(current_time_point().sec_since_epoch());
}
auto boiddonation::get_config(){
  config_table _config(get_self(), get_self().value);
  auto conf = _config.get_or_create(get_self(), config());
  return conf;
}