const eosjs = require('../eosjs')
const env = require('../.env')
const accts = env.accounts
const acct = (name) => accts[env.network][name]
const per = require('./permissions')
const api = eosjs(env.keys[env.network]).api
const tapos = { blocksBehind: 6, expireSeconds: 10 }

async function permissions(){
  try {
    const contract = 'boiddonation'
    const authorization = [{actor:contract,permission: 'active'}]
    const account = 'eosio'
    const result = await api.transact({
      actions: [
        { account, name: "updateauth", data:{
          account:contract, permission:"active", parent:"owner", auth:per.eosioCode(acct('boiddonation'),env.pubkeys[env.network])
        }, authorization },
        { account, name: "updateauth", data:{
          account:acct('token'), permission:"active", parent:"owner", auth:per.donate(acct('boiddonation'),acct('token'),env.pubkeys[env.network])
        }, authorization:[{actor:acct('token'),permission: 'active'}] }
      ]
    },tapos)
    console.log(result.transaction_id)
  } catch (error) {
    return Promise.reject(error.toString())
  }
}

const methods = {permissions}

module.exports = methods

if (process.argv[2]) {
  if (Object.keys(methods).find(el => el === process.argv[2])) {
    console.log("Starting:",process.argv[2])
    methods[process.argv[2]]().catch(console.error)
    .then(()=>console.log('Finished'))
  }
}