import pandas as pd
from pandas.core import algorithms
from pomegranate import *
import numpy as np
import json
import matplotlib.pyplot as plt

#Edit Dataset Paths before Running
print("Please Edit dataset paths before running")
df = pd.read_csv('D:\AAST\Final Project\Final-Project\Software\Hidden Markov Model\dataset.csv')
pd.set_option("display.max_columns", None)
df = df.drop("Time",axis=1)

arr = df['Room'].to_numpy()
arr_state = df['State'].to_numpy()
train_len = len(df.index)
rooms = []
state = []

for i in range(0, train_len, 288):
    sequence = arr[i:i+288]
    sequence2 = arr_state[i:i+288]
    sequence = np.asarray(sequence)
    sequence2 = np.asarray(sequence2)
    sequence = sequence.reshape(1,-1)
    sequence2 = sequence2.reshape(1,-1)
    rooms.append(sequence)
    state.append(sequence2)

rooms = np.asarray(rooms)
state = np.asarray(state)
rooms = rooms.reshape(rooms.shape[0], -1)
state = state.reshape(state.shape[0], -1)

df1 = pd.read_csv('D:\AAST\Final Project\Final-Project\Software\Hidden Markov Model\dataset1.csv')
df1 = df1.drop("Time",axis=1)

arr = df1['Room'].to_numpy()
arr_state = df1['State'].to_numpy()
train_len = len(df1.index)
rooms1 = []
state1 = []

for i in range(0, train_len, 288):
    sequence = arr[i:i+288]
    sequence2 = arr_state[i:i+288]
    sequence = np.asarray(sequence)
    sequence2 = np.asarray(sequence2)
    sequence = sequence.reshape(1,-1)
    sequence2 = sequence2.reshape(1,-1)
    rooms1.append(sequence)
    state1.append(sequence2)

rooms1 = np.asarray(rooms1)
state1 = np.asarray(state1)
rooms1 = rooms1.reshape(rooms1.shape[0], -1)
state1 = state1.reshape(state1.shape[0], -1)

#model = HiddenMarkovModel.from_samples(DiscreteDistribution,n_components = 2,X=rooms,verbose=True)
#model.bake()
with open("D:\AAST\Final Project\Final-Project\Software\Hidden Markov Model\model.json") as json_file1:
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
rooms1 = np.asarray(rooms1)
state1 = np.asarray(state1)
#rooms = np.asarray(rooms)
#state = np.asarray(state)
print(rooms[0])
print("Good Seq: ",model.log_probability(rooms[0]))
print(rooms[5])
print("Good Seq2: ",model.log_probability(rooms[5]))
print(rooms1[50])
print("Bad Seq: ",model.log_probability(rooms1[50]))
#print("Good Score: ", model.score(rooms[0],state[0]))
#print("Bad Score: ", model.score(rooms1[0],state1[0]))

x = []
for i in range(len(rooms)):
    x.append(model.log_probability(rooms[i]))
x = np.asarray(x)

y = []
for i in range(len(rooms1)):
    y.append(model.log_probability(rooms1[i]))
y = np.asarray(y)
plt.hist(x, bins=5)
plt.show()
plt.hist(y, bins=5
)
plt.show()
#print(model.sample(length=288))
