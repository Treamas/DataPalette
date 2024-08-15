import pandas as pd

def get_technique_name():
    return "Clear Formatting"

def get_required_data():
    return ["Target Column"]

def get_parameters():
    cleaning_options = [["Replace Special Characters", "None"]]
    return cleaning_options

def perform_cleaning_technique(table, table_column, method, input):
    # Convert list of lists to DataFrame
    df = pd.DataFrame(table, columns=table_column)

    if method == "Replace Special Characters":
        return df[table_column].replace({r'\\r': '', r'\\n': '', r'\\t': ''}, regex=True)
