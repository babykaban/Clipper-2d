import json
import matplotlib.pyplot as plt

def view_polygons(json_file, index=None):
    """
    Reads polygons from a JSON file and displays them. 
    If an index is provided, displays the polygon pair at that index.
    Otherwise, displays them one by one with interactive scrolling.

    Args:
        json_file: Path to the JSON file containing polygon data.
        index: Optional index of the polygon pair to display.
    """

    with open(json_file, 'r') as f:
        data = json.load(f)

    fig, ax = plt.subplots()
    current_polygon = 0

    def plot_polygon(index):
        ax.clear()
        # Plot first polygon
        polygon_pair = data['polygons'][index]
        case = polygon_pair['case']
        points1 = polygon_pair['points']['subject']
        x1 = [point[0] for point in points1]
        y1 = [point[1] for point in points1]
        x1.append(x1[0])
        y1.append(y1[0])
        ax.plot(x1, y1, label="Subject")
        ax.plot(x1, y1, 'ro', markersize=8)

        # Plot second polygon
        points2 = polygon_pair['points']['clip']
        x2 = [point[0] for point in points2]
        y2 = [point[1] for point in points2]
        x2.append(x2[0])
        y2.append(y2[0])
        ax.plot(x2, y2, label="Clip")
        ax.plot(x2, y2, 'bo', markersize=8)  # Highlight with blue dots

        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_title(f'Polygons {index+1} /{len(data["polygons"])}: {case}')
        ax.legend()
        fig.canvas.draw()

    def key_press(event):
        nonlocal current_polygon
        if event.key == 'right':
            current_polygon = (current_polygon + 1) % len(data['polygons'])
        elif event.key == 'left':
            current_polygon = (current_polygon - 1) % len(data['polygons'])
        plot_polygon(current_polygon)

    if index is not None:
        # Display the polygon at the given index
        try:
            plot_polygon(index)
            plt.show()
        except IndexError:
            print(f"Invalid index. Please provide an index between 0 and {len(data['polygons']) - 1}")
    else:
        # Interactive scrolling through polygons
        fig.canvas.mpl_connect('key_press_event', key_press)
        plot_polygon(current_polygon)
        plt.show()

# Example usage:
# To view a specif
# ic polygon pair (e.g., index 5):
#view_polygons('d:/Clipper-2d/output/polygons.json', index=37292)

# To view all polygons with interactive scrolling:
view_polygons('d:/Clipper-2d/output/polygons.json')