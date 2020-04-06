const eosjs = require('../eosjs')
const env = require('../.env')
const api = eosjs(env.keys[env.network]).api
const acct = (name) => env.accounts[env.network][name]
const tokens_v = require('./tokens')

const tapos = { blocksBehind: 6, expireSeconds: 10 }

async function doAction(name,data){
  try {
    if (!data) data = {}
    const contract = acct('token')
    const authorization = [{actor:contract,permission: 'active'}]
    const account = contract
    const result = await api.transact({actions: [{ account, name, data, authorization }]},tapos)
    console.log(result.transaction_id)
  } catch (error) {
    console.error(error.toString())
  }
}

async function init(){
  console.log('Starting init...')
  for (token of tokens_v) {
    await doAction('create',{issuer:acct('token'),maximum_supply:"100000000.0000 "+ token.token.sym.split(',')[1]})
    await doAction('issue',{to:acct('token'),quantity:"100000000.0000 "+ token.token.sym.split(',')[1],memo:'lul'})
  }
  console.log('finished')
}

init().catch(console.log)
