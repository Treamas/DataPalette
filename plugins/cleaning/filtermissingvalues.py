import pandas as pd
import numpy as np

def get_technique_name():
    return "Filter Missing Values"

def get_required_data():
    return ["Target Column"]

def get_parameters():
    cleaning_options = [["Imputation", "Selection", ["Mean", "Median", "Mode", "Range"]], ["Deletion", "None"], ["Default", "Specified"]]
    return cleaning_options

def perform_cleaning_technique(table, table_column, method, input):
    # Convert list of lists to DataFrame
    df = pd.DataFrame(table, columns=table_column)

    if method == "Imputation":
        if input == "Mean":
            return df[table_column].fillna(df[table_column].mean())
        elif input == "Median":
            return df[table_column].fillna(df[table_column].median())
        elif input == "Mode":
            return df[table_column].fillna(df[table_column].mode().iloc[0])
        elif input == "Range":
            min_val = df[table_column].min()
            max_val = df[table_column].max()
            range_val = max_val - min_val  # Calculate the range
            return df[table_column].fillna(range_val)
    elif method == "Deletion":
        print(f"Table: {table}, Column: {table_column} Modified Column:{df[table_column].dropna()}")
        return df[table_column].dropna()
    elif method == "Default":
        return df[table_column].fillna(input)

    else:
        print("Invalid method or input specified.")
        return df[table_column]