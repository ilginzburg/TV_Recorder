# TV_Recorder
TV shows records planner
Manage TV translations record schedule.
List of translation is being loaded from .ini file
Each translation consisits of date, channel number, start time, end time, name, rating and if was planned.
There are 3 display modes (tabs): 
1) Translations as loaded from ini file with 'if planned' field corrected. In this mode we can set date and channel to see all
relevant translations and then choose start time and end time for record planning.
2) Planning - here we see all record plannings including date, channel, start/end record time, start/end show time, an 
maximal rating of all shows that partially were included in the record time gap. On this tab there is an option to delete planning.
3) List of all shows that partially were included in the record time gap. This list updated relative to changes in planning.
The program uses SQLite database with 2 tables - for plannings and for shows. 
Log file with all program flow events is being created.
