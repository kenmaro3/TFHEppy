from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn.utils import shuffle

def load_data():
  iris = load_iris()
  X = iris.data
  y = iris.target
  X, y = shuffle(X, y, random_state=0)
  return X, y
