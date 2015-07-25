from bokeh.models import ColumnDataSource, HoverTool, TapTool, Callback
from bokeh.plotting import figure


def plot(data):

    source = ColumnDataSource(data)

    p = figure(plot_height=300, tools='', toolbar_location=None)
    p.circle(
        x='x', y='y', source=source,
        size=20,
    )
    p.add_tools(HoverTool(tooltips='@uid'))
    p.add_tools(TapTool())

    source.callback = Callback(args=dict(source=source), code="""
        // What is the selected node?
        var selection = cb_obj.get('selected');
        var selected = selection['1d'].indices;
        console.log(selected);
        // Who signed the selected node?
        var signed_by = source.get('data')['signedby'];

        // Set the selection to the signees
        var new_selection = selected.concat(signed_by[selected[0]]);
        selection['1d'].indices = new_selection;
        cb_obj.set('selected', selection);
    """)
    return p
