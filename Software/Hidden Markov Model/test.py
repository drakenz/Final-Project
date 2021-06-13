import pandas as pd
from pandas.core import algorithms
from pomegranate import *
import numpy as np
import json

#Edit Dataset Paths before Running
print("Please Edit dataset paths before running")
df = pd.read_csv('G:/generator/dataset.csv')
pd.set_option("display.max_columns", None)
df = df.drop("Time",axis=1)

room_dataset1 = df.values.tolist()
rooms = []
state = []
str_rooms = ""
str_states = ""
for i in range(len(room_dataset1)):
    str_rooms += str(room_dataset1[i][0])
    str_states += str(room_dataset1[i][1])
    if (i + 1) % 288 == 0 and i > 0:
        list_rooms = np.array(list(str_rooms),dtype=int)
        states = np.array(list(str_states),dtype=int)
        rooms.append(list_rooms)
        state.append(states)
        str_rooms = ""
        str_states = ""
df1 = pd.read_csv('G:/generator/dataset1.csv')

df1 = df1.drop("Time",axis=1)

room_dataset2 = df1.values.tolist()
rooms1 = []
state1 = []
str_rooms2 = ""
str_states2 = ""
for j in range(len(room_dataset2)):
    str_rooms2 += str(room_dataset2[j][0])
    str_states2 += str(room_dataset2[j][1])
    if (j + 1) % 288 == 0 and j > 0:
        list_rooms = np.array(list(str_rooms2),dtype=int)
        states = np.array(list(str_states2),dtype=int)
        rooms1.append(list_rooms)
        state1.append(states)
        str_rooms2 = ""
        str_states2 = ""

#model = HiddenMarkovModel.from_samples(DiscreteDistribution,n_components = 2,X=rooms,verbose=True)
#model.bake()
with open("model.json") as json_file1:
    data = json.load(json_file1)
model = HiddenMarkovModel.from_json(data)
#model.fit(rooms,algorithm='baum-welch')
x = 0
while x < 288:
    rooms1[0][x] = 1
    x += 1
print(rooms1[0][0])
'''''
json1 = model.to_json()
with open("model.json" , "w") as json_file:
    data = json.dump(json1 , json_file)
'''
print("Good Seq: ",model.log_probability(rooms[0]))
print("Bad Seq: ",model.log_probability(rooms1[0]))
rooms1 = np.asarray(rooms1)
state1 = np.asarray(state1)
rooms = np.asarray(rooms)
state = np.asarray(state)
#state1 = state1.reshape(-1, )
print(rooms1.shape)
print(state1.shape)
print("Good Score: ", model.score(rooms[0],state[0]))
print("Bad Score: ", model.score(rooms1[0],state1[0]))
#print(model.sample(length=288))
