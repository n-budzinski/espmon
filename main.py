from flask import Flask, request, render_template
from datetime import datetime

class Status:
    def __init__(self) -> None:
        self.__e_values = {
            "r32in" : 0,
            "r32out" : 0,
            "waterin" : 0,
            "waterout" : 0,
            "last_update": None
        }

        self.__p_values = {
            "power" : 0,
            "voltage" : 0,
            "amperage" : 0,
            "energy" : 0,
            "frequency" : 0,
            "last_update" : None
        }

    def set_exchanger(self, dictionary):
        for sensor in dictionary:
            self.__e_values[sensor] = dictionary[sensor]
        self.__e_values["last_update"] = datetime.now().strftime()

    def set_heat_pump(self, dictionary):
        for sensor in dictionary:
            self.__p_values[sensor] = dictionary[sensor]
        self.__p_values["last_update"] = datetime.now().strftime()

    def get_exchanger(self):
        return self.__e_values
    
    def get_heat_pump(self):
        return self.__p_values

status = Status()

app = Flask(
    import_name=__name__,
    static_folder="./static"
    )

@app.route("/update_<string:mode>")
def update(mode):
    if request.args:
        if mode == "exchanger":
            status.set_exchanger(request.args)
        elif mode == "heat_pump":
            status.set_heat_pump(request.args)
        else:
            return "ERR_BAD_MODE"
        return "OK"
    else:
        return "ERR_NO_VALUE"


@app.route("/")
def index():
    return render_template(
        "./index.html",
        e_values = status.get_exchanger(),
        p_values = status.get_heat_pump()
    )


def main():
    app.run(
        host="0.0.0.0",
        debug=True
    )

if __name__ == "__main__":
    main()