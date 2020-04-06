# boiddonation

### How this thing works
The contract manages a list of tokens that users can donate. Each whitelisted token has a minimum donation value which increases slightly with each donation. Most variables are exposed in the config table. The token whitelist can be updated and each token can be configured.

Each token is configured to generate power at a specific set rate (roughly based on the market liquidity of the token). Generated Boid Power accumulates in the token pool. When users donate a specific token, they can receive some Boid Power which is subtracted from the token power pool. If power is distributed as donation rewards at a higher rate than it is replenished, then the available power can decrease to 0 and that specific token can no longer provide power rewards until the pool recharges.

Each user has a global metric which is the total number of donations made (the value of the donation doesn't matter for this metric). Each user has a "Level" which increases as the user makes more donations (the default is to level up every 10 transactions made with a max of 10 levels). Each time an account levels up, the base power reward they receive from a donation increases (default is by 10 points) up to a maximum (default 100 Boid Power at level 10). 

Each token pool has a maximum power capacity and the available power will not increase past this capacity. As the power capacity reaches the maximum threshold, a power multiplier increases (default max is 5x at 100% power pool capacity). This is to incentivize users to contribute to a pool with less recent contributions. 100 is the max base power (account has reached level 10), but combined with the per token pool multiplier that means you could receive up to 500 Boid Power per donation made. 

The contract also has start/end time stored as utc ms. You can only make a donation in between start/end. The idea here is to reward the top contributors with rare NFTs and physical rewards while lowering the barrier to entry for generating Boid Power during the season break. We hope the donation event can be a gateway for new users to learn about Boid and get comfortable using the platform.

### live on kylin
Contract: boiddonation

### Live on EOS
Contract: boiddonation
