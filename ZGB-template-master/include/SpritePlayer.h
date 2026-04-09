// CUSTOM_DATA usage
#define recharge_time_lo 0
#define recharge_time_hi 1
#define death_state 2
#define movement_accumulator 3
#define death_timer 6
#define death_step 4
#define death_base_y 5

#define playerDeathNone 0
#define playerDeathBounce 1
#define playerDeathPreGraveWait 2
#define playerDeathBagPinned 3

BOOLEAN crushPlayerWithBag(uint16_t bagY) BANKED;
void finalizePlayerBagCrush(void) BANKED;
