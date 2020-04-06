# boiddonation

### How this thing works
The contract manages a list of tokens that users can donate. Each whitelisted token has a minimum donation metric which slowly increases over time (default is 1% of each contribution or 1% of the previous minimum for that token whichever is less). 

Each token type also generates Boid Power at a specific rate. When users donate a specific token, they can earn some amount of Boid Power which is subtracted from that token's available power pool. If power is distributed as donation rewards at a higher rate than it is replenished, then the available power can decrease to 0 and that token no longer provides power rewards ( users should wait for it to recharge). 

Each token pool has a maximum power capacity, the available power pool will not increase past this capacity, additionally, as the power capacity reaches the maximum threshold, a power multiplier increases (default is up to 500% bonus power if the token power is near max capacity). This is to incentivize users to contribute to a pool with less recent contributions. 

When users make a contribution, the contract tracks how many donations they made of each type of token. Each user has a global metric which is the total number of donations made (the value of the donation doesn't matter for this metric). Additionally each user has a "Level" which increases as the user makes more donations (the default is to level up every 10 transactions made with a max of 10 levels) Each time an account levels up, the base power reward they receive from a donation increases (default is by 10 points) up to a maximum (default 100). 

100 is the max base power, but combined with the multiplier that means you could get up to 500 Boid Power per donation made. The contract also has start/end utc time, you can only make a donation in between start/end. The idea here is to reward the top contributors with rare NFTs and physical rewards as well as lowering the barrier to entry for generating Boid Power during the season break. 

### live on kylin
Contract: boiddonation
Tokens: boiddummytkn
Keys: same as token.boid

for debugging the contract time is accelerated: 1 hr = one day.
