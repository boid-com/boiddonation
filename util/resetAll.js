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
  console.log('Starting reset...')
  // await doAction('clearconfig')
  // await doAction('cleartokens')
  // await doAction('clearcontrib',{rows:10000})
  // await doAction('initconfig')
  // await doAction('addtokens',{tokens_v:tokens_v.map(el =>{
  //   if (env.network === 'kylin') {
  //     el.token.contract = 'boiddummytkn'
  //     if (el.token.sym.split(',')[1] == "BOID") el.token.contract = 'token.boid'
  //   }
  //   return el
  // })})
  await doAction('setconfig',{conf:config})
  console.log('Reset finished')
}

init().catch(console.log)
