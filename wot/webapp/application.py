#! /usr/bin/env python

import datetime

from flask import Flask, request, render_template
from bokeh.embed import components

from make_plot import plot
from utils import GPG

app = Flask(__name__)


@app.route("/")
def main(date=None):
    data = {
        'x': [0, 1, 1, 0, 1],
        'y': [1, 2, 1, 2, 1.5],
        'uid': ['Fedora 22 signing key', 'Petr Pisar <ppisar@redhat.com>', 'a', 'b', 'c'],
        'signedby': [[1, 2], [], [0, 1, 2], [0], [0]]
    }
    p = plot(data)
    script, div = components(p)
    rendered_html = render_template('index.html', div=div, script=script)
    return rendered_html


if __name__ == "__main__":
    app.run(debug=True)
