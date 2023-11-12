# TSAT_P_P_001
TSAT P Prototype v001

Astronode S+ is the only Communication (COMMN) element. If COMMN is not working, system restart is used.

LIS2DW12 accelerometer (ACC) is the only movement detection element. If ACC is not working, system is periodicaly (RTC events based) check locations and uses COMMN. So the ACC function is not critical and restart is not used.
