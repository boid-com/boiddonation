const eosjs = require('../eosjs')
const env = require('../.env')
const api = eosjs(env.keys[env.network]).api
const acct = (name) => env.accounts[env.network][name]
const tokens_v = require('./tokens')

const tapos = { blocksBehind: 6, expireSeconds: 10 }

async function doAction(name,data){
  try {
    if (!data) data = {}
    const contract = data.from
    const authorization = [{actor:contract,permission: 'active'}]
    const account = contract
    const result = await api.transact({actions: [{ account, name, data, authorization }]},tapos)
    console.log(result.transaction_id)
  } catch (error) {
    console.error(error.toString())
  }
}

const receivers = ["chintaifever","sethfromboid","boid1hunters","john.boid"]


async function init(){
  console.log('Starting init...')
  for (receiver of receivers) {
    for (token of tokens_v) {
      const tkn = token.token.sym.split(',')[1]
      var from
      if (tkn === "BOID") from = acct('boidtoken')
      else from = acct('token')
      await doAction('transfer',{memo:"get it",from, to:receiver,quantity:"100000.0000 "+ tkn})
    }
  }

  console.log('finished')
}

init().catch(console.log)
