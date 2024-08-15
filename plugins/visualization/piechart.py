import matplotlib.pyplot as plt

def get_visualization_name():
    return "Pie Chart"

def get_required_data():
    return ["x_column", "y_column"]

def get_parameters():
    return [
        "pie works",
        "x_label",
        "y_label",
        "legend_labels"
    ]

def create_visualization(data, parameters):
    # Extract the data from the dictionary
    labels = data['x_column']
    sizes = data['y_column']

    # Create a figure and axis object
    fig, ax = plt.subplots()

    # Plot the data as a pie chart
    ax.pie(sizes, labels=labels, autopct='%1.1f%%')
    ax.axis('equal')  # Equal aspect ratio ensures that pie is drawn as a circle.

    # Set the title based on the parameters
    if len(parameters) >= 1:
        ax.set_title(parameters[0])

    # Show the plot
    plt.show()