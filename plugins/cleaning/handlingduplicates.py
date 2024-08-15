import pandas as pd
import numpy as np
from scipy.stats import norm

def get_technique_name():
    return "Handling Duplicates"

def get_required_data():
    return ["Target Column"]

def get_parameters():
    cleaning_options = [
        ["Standard Deviation", "Specified"],
        ["Percentile", "Specified"],
        ["IQR", "None"],
        ["Mahalanobis Distance", "None"],
        ["Isolation Forest", "None"],
        ["Domain Knowledge", "Specified"]
    ]
    return cleaning_options

def perform_cleaning_technique(table, table_column, method, input=None):
    df = pd.DataFrame(table, columns=table_column)

    if method == "Standard Deviation":
        mean = df[table_column].mean()
        std = df[table_column].std()
        threshold = float(input)  # Assuming the input is a numerical value
        return df[(np.abs(df[table_column] - mean) <= threshold * std)]

    elif method == "Percentile":
        lower_threshold = float(input)  # Assuming the input is a numerical value
        upper_threshold = 100 - lower_threshold
        return df[(df[table_column] >= df[table_column].quantile(lower_threshold / 100)) &
                  (df[table_column] <= df[table_column].quantile(upper_threshold / 100))]

    elif method == "IQR":
        q1 = df[table_column].quantile(0.25)
        q3 = df[table_column].quantile(0.75)
        iqr = q3 - q1
        lower_bound = q1 - 1.5 * iqr
        upper_bound = q3 + 1.5 * iqr
        return df[(df[table_column] >= lower_bound) & (df[table_column] <= upper_bound)]

    elif method == "Mahalanobis Distance":
        covariance = df.cov()
        mean_vector = df.mean()
        mahalanobis_distances = [(x - mean_vector).dot(np.linalg.inv(covariance)).dot((x - mean_vector).T)
                                 for x in df.values]
        threshold = 3  # Adjust as desired
        return df[np.array(mahalanobis_distances) <= threshold ** 2]

    elif method == "Isolation Forest":
        from sklearn.ensemble import IsolationForest
        model = IsolationForest()
        model.fit(df)
        outlier_mask = model.predict(df) == -1
        return df[~outlier_mask]

    elif method == "Domain Knowledge":
        try:
            threshold = float(input)
            return df[(df[table_column] >= threshold) & (df[table_column] <= threshold)]
        except ValueError:
            print("Invalid input for Domain Knowledge method. Please provide a numerical value.")
            return df[table_column]

    else:
        print("Invalid method specified.")
        return df[table_column]
        