const eosjs = require('../eosjs')
const env = require('../.env')
const api = eosjs(env.keys[env.network]).api
const acct = (name) => env.accounts[env.network][name]
const tokens_v = require('./tokens')
const config = require('./config')
const tapos = { blocksBehind: 6, expireSeconds: 10 }

async function doAction(name,data){
  try {
    if (!data) data = {}
    const contract = acct('boiddonation')
    const authorization = [{actor:contract,permission: 'active'}]
    const account = contract
    const result = await api.transact({actions: [{ account, name, data, authorization }]},tapos)
    console.log(result.transaction_id)
  } catch (error) {
    console.error(error.toString())
  }
}

async function init(){
  await doAction('erasetoken',{token_symbol:"4,ATMOS",token:{token:{contract:"novusphereio",sym:"3,ATMOS"},quantity:900,daily_power_regen:200,power_available_cap:6000,current_power_available:500}})
  // await doAction('erasetoken',{token_symbol:"4,ATMOS",token:{token:{contract:"novusphereio",sym:"3,ATMOS"},quantity:900,daily_power_regen:200,power_available_cap:6000,current_power_available:500}})

}

init().catch(console.log)
