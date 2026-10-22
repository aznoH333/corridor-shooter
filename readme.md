# how to compile
linus:
```bash
$ make
```

mac:
```bash
$ make mac
```

win not supported yet :(

# todo:
- [X] Entity rotation
- [X] Particle splatter
- [X] Fix shader not having a color depth buffer
- [X] Gore bits
- [X] fix invincible enemies
- [-] incendiary rounds effect
- [-] stun rounds effect
- [?] enemy armor system
- [X] gun weight
- [X] rework recoil to look more natural
- [ ] customisable bullet trails
- [X] magazines and ammo counts
- [ ] explosives and explosive damage
- [ ] spin up delay for the miniguns


# high prio stuff
- [X] Rework enemy ais
- [X] Enemy attacks
- [ ] Ranged enemy variants
- [ ] Rebalance gun damage values to match the enemy stats
- [X] Health and dying
- [ ] UI system

# long term goals
- [ ] World gen
- [X] Hud
- [ ] Level continuity
- [ ] Menu + ui
- [ ] Some way to get guns


# notes
- get rid of unimplemented bullet variats 🤔. Can always get back to that later.
- should focus more on gameplay mechanics instead of minor details
- 2 ish bullet types and maybe a receiver and the weapon system can be considered "done"






# super short term goals
[X] add clear state function
[X] wire menu transitions to clear the state
[X] make a system that spawn enemies when the player walks past a certain point
[ ] make enemies indexed by their names
[ ] make an enemy table that ties enemies to difficulity
[ ] wire the enemy difficulity table into the spawn system
[ ] make gd able to spawn groups of enemies
[ ] make gd periodically spawn enemy patrols
[ ] make a function that assesses how good the player is doing (health+resources = some score)
[ ] make gd scale based on player score


