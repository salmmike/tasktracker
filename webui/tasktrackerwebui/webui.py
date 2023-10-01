from flask import Flask, render_template, request
import datetime
import time
import requests
from enum import IntEnum

from waitress import serve

TASK_POST_ADDR = "http://127.0.0.1:8181/task"

INPUT_TASK_HTML = "input_task.html"


class RepeatInfo(IntEnum):
    NoRepat = 0
    Monthly = 1
    MonthlyDay = 2
    SpecifiedDays = 3
    WithInterval = 4


def get_repeat_info(info: str):
    """
    Repeat info enum to values.
    """
    if info == "daily":
        return RepeatInfo.SpecifiedDays, 1234567
    if info == "weekly":
        return RepeatInfo.WithInterval, 7
    if info == "weekdays":
        return RepeatInfo.SpecifiedDays, 12345
    if info == "biweekly":
        return RepeatInfo.WithInterval, 14
    raise RuntimeError("Wrong repeat info!")


def post_add_task(data: dict):
    """
    Add task to tasktracker
    """
    try:
        start_date = [int(x) for x in data.get("task_start").split("-")]
        if len(start_date) != 3:
            raise ValueError()
    except ValueError:
        raise RuntimeError("Invalid value for start date!")
    try:
        start_time = [int(x) for x in data.get("task_time").split(":")]
        if len(start_time) != 2:
            raise ValueError()
    except ValueError:
        raise RuntimeError("Invalid value for start time!")

    if not data.get("task_name"):
        raise RuntimeError("No task name!")

    if len(start_time) != 2:
        raise ValueError("start_time parameter incorrect")
    if len(start_date) != 3:
        raise ValueError("start_date parameter incorrect")

    time_t = datetime.datetime(
        year=start_date[0],
        month=start_date[1],
        day=start_date[2],
        hour=start_time[0],
        minute=start_time[1],
    )

    time_t = int(time.mktime(time_t.timetuple()))
    repeat_type, repeat_info = get_repeat_info(data.get("repeat_info"))

    jsondata = {
        "taskName": data.get("task_name"),
        "taskStart": time_t,
        "taskRepeatInfo": repeat_info,
        "taskRepeatType": repeat_type,
    }

    resp = requests.post(url=TASK_POST_ADDR, json=jsondata, verify=False)
    if resp.status_code != 200:
        raise RuntimeError(resp.text)


def make_app() -> Flask:
    app = Flask(__name__)

    @app.route("/", methods=["POST", "GET"])
    def create_task():
        try:
            if request.method == "GET":
                return render_template(INPUT_TASK_HTML)
            if request.method == "POST":
                print(request.form.to_dict())
                post_add_task(request.form.to_dict())
                return render_template(INPUT_TASK_HTML)
        except Exception as err:
            return str(err)

    return app


def start():
    app = make_app()
    serve(app, host="0.0.0.0", port=5000)


if __name__ == "__main__":
    start()
