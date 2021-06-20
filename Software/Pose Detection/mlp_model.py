import math
import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from keras.models import Sequential
from keras.layers.core import Dense,Activation,Dropout
from keras.optimizers import SGD, Adam, Nadam, RMSprop
from sklearn.metrics import accuracy_score
import matplotlib.pyplot as plt
from sklearn.preprocessing import LabelEncoder
from sklearn.preprocessing import StandardScaler
import talos
from talos.model import lr_normalizer, hidden_layers
from keras.metrics import Precision, Recall

def round_half_up(n, decimals=0):
    multiplier = 10 ** decimals
    return math.floor(n*multiplier + 0.5) / multiplier
round_half_up_v = np.vectorize(round_half_up)

df = pd.read_csv("images_scores.csv")
df['Label'] = pd.factorize(df['Label'])[0]
df = df.drop(df.columns[[0]],axis=1)
print(df)

x = df.iloc[:, 1:].values
y = df.iloc[:, 0].values
print(x.shape)
print(y.shape)
'''
encoder = LabelEncoder()
encoder.fit(y)
encoded_Y = encoder.transform(y)
dummy_y = np_utils.to_categorical(encoded_Y)
print(dummy_y)
print(dummy_y.shape)
print(y)
'''
#scaler = StandardScaler().fit(x)
#X_scaled = scaler.transform(x)
#print(scaler.get_params())
#print(X_scaled.mean())
#print(X_scaled.std())

#x = np.asarray(X_scaled)

#y = np.asarray(dummy_y)

trainX, X_notTrain, trainY, y_notTrain = train_test_split(x, y, test_size=0.5, random_state=9)
testX, valX, testY, valY = train_test_split(X_notTrain, y_notTrain, test_size=0.5, random_state=42)
scaler = StandardScaler().fit(trainX)
trainX = scaler.transform(trainX)
testX = scaler.transform(testX)
valX = scaler.transform(valX)

file = open("data.txt", "w")
for row in trainX:
    np.savetxt(file, row)
file.close()

optm = Adam(lr=0.001)
model=Sequential()
model.add(Dense(16,input_dim=trainX.shape[1], activation='elu', kernel_initializer= 'random_normal', bias_initializer='zeros', use_bias=True))
model.add(Dropout(0.2))
model.add(Dense(1, activation='sigmoid',kernel_initializer='random_normal', bias_initializer='zeros', use_bias=True))

model.compile(loss="logcosh", optimizer=optm, metrics=['acc'])
history = model.fit(trainX, trainY,
                        validation_data=(valX, valY),
                        batch_size=13,
                        epochs=150)

'''
opt = Adam(lr=0.01)
model.compile(loss='categorical_crossentropy', optimizer=opt , metrics=['accuracy'])

history = model.fit(trainX, trainY, validation_data=(valX, valY), epochs=150, batch_size=32)
'''

y_predict = model.predict(testX)
predicted_y = round_half_up_v(y_predict)
print(accuracy_score(testY, predicted_y))

scores=model.evaluate(testX,testY)
print("Accuracy: " , scores[1]*100, "%")
print("Loss: " , scores[0]*100, "%")

plt.plot(history.history['acc'])
plt.plot(history.history['val_acc'])
plt.title('model accuracy')
plt.ylabel('accuracy')
plt.xlabel('epoch')
plt.legend(['train', 'test'], loc='upper left')
plt.show()

plt.plot(history.history['loss'])
plt.plot(history.history['val_loss'])
plt.title('model loss')
plt.ylabel('loss')
plt.xlabel('epoch')
plt.legend(['train', 'test'], loc='upper left')
plt.show()

model.save("Model4.h5")
