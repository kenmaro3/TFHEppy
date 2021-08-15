import keras
from keras.layers import Dense, Dropout, Flatten, Lambda, Activation
from keras import backend as K
from keras.models import Sequential, model_from_json
from keras.layers.core import Dense
from keras.utils import np_utils
#from keras.optimizers import Adam
from keras.callbacks import CSVLogger
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
import sklearn
import tensorflow as tf


def dispense_data():
    iris = load_iris()

    data_x = iris.data
    data_y = iris.target
    data_y = np_utils.to_categorical(data_y)
    data_x = sklearn.preprocessing.normalize(data_x)

    x_train, x_test, y_train, y_test = train_test_split(data_x, data_y, test_size=0.2)

    return x_train, y_train, x_test, y_test


def dispense_model():
    model = Sequential()
    model.add(Dense(input_dim=4, units=100, activation='relu'))
    model.add(Dense(input_dim=100, units=32, activation='relu'))
    model.add(Dense(input_dim=32, units=3))
    model.add(Activation('softmax'))
    model.compile(loss='categorical_crossentropy', optimizer=tf.keras.optimizers.Adam(), metrics=['accuracy'])

    return model


def train_model(model, model_path, x_train, y_train, x_test, y_test):
    history = model.fit(x_train, y_train,
                        batch_size=32, epochs=40,
                        verbose=1,
                        validation_data=(x_test, y_test),
                        )
    model.save(model_path)

    return model




if __name__ == "__main__":

    model_name = "nn_iris"
    model_path = f'{model_name}.h5'

    x_train, y_train, x_valid, y_valid = dispense_data()

    model = dispense_model()
    model = train_model(model, model_path, x_train, y_train, x_valid, y_valid)

