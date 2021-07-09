import datetime
import random
from dateutil.relativedelta import relativedelta
import pandas as pd
import csv


class Person():
    def __init__(self, month):
        self.month = month
        self.start = datetime.datetime.now() - relativedelta(months=self.month)
        self.start = self.start.replace(
            hour=0, minute=0, second=0, microsecond=0)

        self.end = self.start + relativedelta(months=1)
        self.dataset = []

        self.state = 0
        self.awake = 1

    def print(self):
        print(self.start)
        print(self.end)
        print(self.state)
        print(self.awake)
        print(self.dataset)

    def daily_routine(self):
        # First is Rounding the time
        # Create a list of list , each item in the main list has a list of one day
        dataset = []

        wakeup_flag = 8
        sleep_flag = 0
        is_sleeping = False
        sleeping_room = 1
        Living = 2
        Bathroom = 3

        start_var = self.start
        period = self.month * 30
        for i in range(period):  # period
            day = []
            start_copy = self.start
            SleepTime, WakeupTime, BackFromWork, OtherRooms = self.generateDay()

            while(self.start < WakeupTime):
                day.append([str(self.start), 1, 0])
                self.start = self.start + datetime.timedelta(minutes=5)

            day.append([str(self.start), 1, 1])
            self.start = self.start + datetime.timedelta(minutes=5)
            day.append([str(self.start), 4, 1])
            self.start = self.start + datetime.timedelta(minutes=5)

            while(self.start < BackFromWork):
                day.append([str(self.start), 0, 0])
                self.start = self.start + datetime.timedelta(minutes=5)
            for i in OtherRooms:
                while(self.start < i[1]):
                    # TODO: Randomise 1 (Not Sleeping) somehow
                    day.append([str(self.start), i[0], 1])
                    self.start = self.start + datetime.timedelta(minutes=5)

            while(self.start.hour == 23 and self.start.minute <= 55):
                day.append([str(self.start), 1, 0])
                self.start = self.start + datetime.timedelta(minutes=5)

            self.dataset.append(day)

            self.start = start_copy + datetime.timedelta(days=1)
            self.start = self.start.replace(
                hour=0, minute=0, second=0, microsecond=0)
        self.toCSV()

        # Sleep at 11 ~ 12 PM (Previous day)
        # Wake up at 7 ~ 8 AM (law 3ayz add one state awake bathroom)
        # after wake up time 1 * awake bedroom , 1 * standing bathroom then all states 0 (work)
        # Work 5 ~ 6 PM (All states 0)
        # Randomly Bedroom, Living, Kitchen for 30-60 mins until 11 ~ 12 pm
        # Sleep Time 11 ~ 12 PM

        # 2 Random Bathroom States Any time ! work

    def generateDay(self):
        WakeupTime = self.start + \
            datetime.timedelta(hours=7) + \
            datetime.timedelta(minutes=random.randint(0, 60))
        BackFromWork = WakeupTime + \
            datetime.timedelta(hours=8) + \
            datetime.timedelta(minutes=random.randint(0, 60))

        # Start Activity = BackFromWork + Margin time (Max 18)
        SleepTime = self.start + \
            datetime.timedelta(hours=23) + \
            datetime.timedelta(minutes=random.randint(0, 59))
        remaining_time = SleepTime - BackFromWork
        # remaining_time = datetime.timedelta(hours=remaining_time., minutes=SleepTime.minute,seconds=SleepTime.second)
        Activity_Period = BackFromWork
        ListOfOtherRooms = []
        # print(remaining_time)
        while(remaining_time > datetime.timedelta(hours=0, minutes=0, seconds=0)):
            # print(DayEnd)
            # print(remaining_time)
            Room = random.randint(1, 3)
            Time = datetime.timedelta(minutes=random.randint(30, 60))
            ListOfOtherRooms.append([Room, Activity_Period + Time])
            Activity_Period = Activity_Period + Time
            remaining_time -= Time

        ListOfOtherRooms[-1] = [1, SleepTime]
        return SleepTime, WakeupTime, BackFromWork, ListOfOtherRooms

    def printDayTimes(self):
        SleepTime, WakeupTime, BackFromWork, OtherRooms = self.generateDay()
        # self.generateBathroom(OtherRooms)
        print("Sleeping At : ", SleepTime)
        print("Waking up At : ", WakeupTime)
        print("Back From Work At : ", BackFromWork)
        for i in OtherRooms:
            print(i)

    def generateBathroom(self, Scheduele):  # TODO : Make this after finishing states
        length_of_list = len(Scheduele)
        item = random.randint(0, length_of_list)
        Scheduele[item][0] = 4

    def toCSV(self):
        fieldname = ['Time', 'Room', 'State']

        file = open('dataset.csv', 'a+', newline='')
        with file:
            write = csv.writer(file)
            write.writerow(fieldname)

            for i in self.dataset:
                write.writerows(i)

        #dataset = self.dataset
        #df = pd.DataFrame(dataset)
        #df.to_csv("dataset.csv",header=["Date", "Room", "State"],index=False)


# Posture:
'''
0 Sleeping
1 Awake
'''

# Rooms
'''
0: Nothing
1: Bedroom
2: Living
3: Kitchen
4: Bathroom
'''


'''
[3 0], [3 0], [3 1], [3 1], [5 1], [4 1], [4 0], [4 1], [4 1], [4 1], [5 1], [3 1], [3 0], [3 0], [3 1], [5 1], [1 1], [1 1], [1 1], [1 1], [8 1], [8 1], [1 0], [1 0], [1 0
TIME    ROOM Posture
+5      0    0
+5      2    1
+5      5    1
+5      0    0
+5      3    1
+5      4    1
'''
