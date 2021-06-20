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
from keras.utils import np_utils, to_categorical
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
scaler = StandardScaler().fit(x)
X_scaled = scaler.transform(x)
print(X_scaled.mean())
print(X_scaled.std())

x = np.asarray(X_scaled)

#y = np.asarray(dummy_y)

trainX, X_notTrain, trainY, y_notTrain = train_test_split(x, y, test_size=0.4, random_state=9)
testX, valX, testY, valY = train_test_split(X_notTrain, y_notTrain, test_size=0.5, random_state=42)

p = {'lr': (0.5, 5, 10),
     'first_neuron':[4, 8, 16, 32, 64],
     'hidden_layers':[0, 1, 2],
     'batch_size': (2, 30, 10),
     'epochs': [150],
     'dropout': (0, 0.5, 5),
     'weight_regulizer':[None],
     'emb_output_dims': [None],
     'shapes':['brick','funnel'],
     'optimizer': [Adam, Nadam, RMSprop],
     'losses': ['logcosh', 'binary_crossentropy'],
     'activation':['relu', 'elu'],
     'last_activation': ['sigmoid']}

def pose_detection_model(trainX, trainY, valX, valY, params):

    model=Sequential()
    model.add(Dense(params['first_neuron'],input_dim=trainX.shape[1], activation=params['activation'], kernel_initializer= 'normal'))
    model.add(Dropout(params['dropout']))
    hidden_layers(model,params,1)
    model.add(Dense(1, activation=params['last_activation'],kernel_initializer='normal'))
    model.add(Dense(40,activation='relu'))
    model.add(Dropout(0.2))
    model.add(Dense(20,activation='relu'))
    model.add(Dropout(0.2))
    model.add(Dense(2,activation='softmax'))
    model.compile(loss=params['losses'], optimizer=params['optimizer'](lr=lr_normalizer(params['lr'],params['optimizer'])), metrics=['acc'])
    history = model.fit(trainX, trainY,
                        validation_data=(valX, valY),
                        batch_size=params['batch_size'],
                        epochs=params['epochs'],
                        verbose=0)
    return history, model

t = talos.Scan(x=x,
               y=y,
               model=pose_detection_model,
               params=p,
               fraction_limit=0.01,
               experiment_name='pose_score',
               round_limit=10,
               x_val=valX,
               y_val=valY)
'''
opt = Adam(lr=0.01)
model.compile(loss='categorical_crossentropy', optimizer=opt , metrics=['accuracy'])

history = model.fit(trainX, trainY, validation_data=(valX, valY), epochs=150, batch_size=32)

y_predict = model.predict(testX)
predicted_y = round_half_up_v(y_predict)
print(accuracy_score(testY, predicted_y))

scores=model.evaluate(testX,testY)
print("Accuracy: " , scores[1]*100, "%")
print("Loss: " , scores[0]*100, "%")

plt.plot(history.history['accuracy'])
plt.plot(history.history['val_accuracy'])
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

model.save("Model.h5")
'''