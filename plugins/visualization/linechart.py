import matplotlib.pyplot as plt

def get_visualization_name():
    return "Line Chart"

def get_required_data():
    return ["x_column", "y_column"]

def get_parameters():
    return [
        "title",
        "x_label",
        "y_label",
        "legend_labels"
    ]

def create_visualization(data, parameters):
    # Extract the data from the dictionary
    x_data = data['x_column']
    y_data = data['y_column']

    # Create a figure and axis object
    fig, ax = plt.subplots()

    # Plot the data
    ax.plot(x_data, y_data)

    # Set the title and axis labels based on the parameters
    if len(parameters) >= 1:
        ax.set_title(parameters[0])
    if len(parameters) >= 3:
        ax.set_xlabel(parameters[1])
        ax.set_ylabel(parameters[2])

    # Show the plot
    plt.show()