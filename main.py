from flask import Flask, request, render_template
from json import dumps
from typing import Dict

class Status:
    def __init__(self) -> None:
        self.__values = {
            "r32_in" : 0,
            "r32_out" : 0,
            "water_in" : 0,
            "water_out" : 0,
            "power" : 0,
            "voltage" : 0,
            "amperage" : 0,
            "energy" : 0,
            "frequency" : 0
        }

    def set_values(self, dictionary: Dict[str, str]) -> None:
        for sensor in dictionary:
            if sensor in self.__values:
                if dictionary[sensor].isnumeric():
                    self.__values[sensor] = float(dictionary[sensor]) # type: ignore
                else:
                    self.__values[sensor] = dictionary[sensor] # type: ignore

    def get_values(self) -> Dict:
        return self.__values

status = Status()

app = Flask(
    import_name=__name__,
    static_folder="./static"
    )

@app.route("/set")
def setter():
    if request.args:
        status.set_values(request.args)
        return "OK"
    else:
        return "NO_VALUE"

@app.route("/")
def index():
    return render_template(
        "./index.html",
        variables = dumps(status.get_values()),
    )

@app.route("/update")
def update():
    return dumps(status.get_values())

def main():
    app.run(
        host="0.0.0.0",
        debug=True
    )

if __name__ == "__main__":
    main()