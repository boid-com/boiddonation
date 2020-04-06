module.exports = {
  eosioCode:(contract,key) => JSON.parse(`
  {
    "threshold" : 1,
    "keys" : [{"key":"${key}", "weight":1}],
    "accounts": [{"permission":{"actor":"${contract}", "permission":"eosio.code"}, "weight":1}],
    "waits": []
  }
  `),
  donate:(contract,tokencontract,key) => JSON.parse(`
  {
    "threshold" : 1,
    "keys" : [{"key":"${key}", "weight":1}],
    "accounts": [{"permission":{"actor":"${contract}", "permission":"active"}, "weight":1},{"permission":{"actor":"${tokencontract}", "permission":"eosio.code"}, "weight":1}],
    "waits": []
  }
  `)
}