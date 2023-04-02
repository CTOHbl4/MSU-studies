import os

from sklearn.ensemble import RandomForestRegressor
from sklearn.pipeline import Pipeline

import numpy as np


class PotentialTransformer:

    def fit(self, x, y):
        return self

    def fit_transform(self, x, y):
        return self.transform(x)

    def transform(self, x):
        transformed = []
        for matr in x:
            transformed.append(np.concatenate((np.sum(matr, axis=0), np.sum(matr, axis=1))))
        transformed = np.array(transformed)
        return transformed


def load_dataset(data_dir):
    files, X, Y = [], [], []
    for file in os.listdir(data_dir):
        potential = np.load(os.path.join(data_dir, file))
        files.append(file)
        X.append(potential["data"])
        Y.append(potential["target"])
    return files, np.array(X), np.array(Y)


def train_model_and_predict(train_dir, test_dir):
    _, X_train, Y_train = load_dataset(train_dir)
    test_files, X_test, Y_test = load_dataset(test_dir)
    regressor = Pipeline(
        [('transformer', PotentialTransformer()),
         ('regressor', RandomForestRegressor(n_estimators=1700, random_state=42, max_features='log2'))])
    regressor.fit(X_train, Y_train)
    predictions = regressor.predict(X_test)
    return {file: value for file, value in zip(test_files, predictions)}
