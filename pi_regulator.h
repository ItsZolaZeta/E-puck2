#ifndef PI_REGULATOR_H
#define PI_REGULATOR_H

//start the PI regulator thread
void pi_regulator_start(void);
void p_regulator_start(void);
int16_t correction(int16_t position_goal);

#endif /* PI_REGULATOR_H */
