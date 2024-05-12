#include "WeatherListModel.h"
#include <stdio.h>

std::string
read_weather(const std::string& command,
             std::string& location,
             std::string& utc_diff)
{
    std::stringstream ss;
    char buffer[2048];
    FILE* in;
    auto cmd =
      (command + " --csv --timezone " + utc_diff + " --city " + location);
    if (!(in = popen(cmd.c_str(), "r"))) {
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), in) != NULL) {
        ss << buffer;
    }
    pclose(in);
    return ss.str();
}

static std::vector<std::string>
split(std::string parsestr, std::string delimiter, bool has_header=false)
{
    std::vector<std::string> output_lines;

    size_t pos = 0;
    std::string token;
    bool header = has_header;

    while ((pos = parsestr.find(delimiter)) != std::string::npos) {
        token = parsestr.substr(0, pos);
        if (!header) {
            output_lines.push_back(token);
        } else {
            header = false;
        }
        parsestr.erase(0, pos + delimiter.length());
    }
    return output_lines;
}

static std::vector<std::vector<std::string>>
split_api_output(std::string parsestr)
{
    std::vector<std::vector<std::string>> output;
    auto lines = split(parsestr, "\n", true);
    for (auto line : lines) {
        output.push_back(split(line, ","));
    }
    return output;
}

QString
get_time_value(const std::string& data)
{
    return QString::fromStdString(split(data, " ").at(0));
}

QString
get_date_value(const std::string& data)
{
    return QString::fromStdString(split(data, " ").at(1));
}

WeatherListModel::WeatherListModel(std::filesystem::path weather_csv_program,
                                   std::string location,
                                   int utc_diff,
                                   QObject* parent)
  : QAbstractListModel{ parent }
  , m_weather_program{ weather_csv_program }
  , m_location{ location }
  , m_utc_diff{ utc_diff }
{
    m_timer = new QTimer(parent);
    m_timer->setInterval(m_refresh_time_s * 1000);
    connect(m_timer, &QTimer::timeout, this, &WeatherListModel::populate);
    m_timer->start();
}

int
WeatherListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_api_output_lines.size() - 1;
}

QString
get_rain_ammount(const std::vector<std::vector<std::string>> &data, int index)
{
    float previous = 0;
    if (index > 0) {
        previous = std::atof(data.at(index - 1).at(4).c_str());
    }
    float current = std::atof(data.at(index).at(4).c_str());
    QString text;
    QTextStream stream{&text};
    stream.setRealNumberPrecision(2);
    stream << (current - previous);

    return text;
}

QVariant
WeatherListModel::data(const QModelIndex& index, int role) const
{
    // time,date,temperature,rain,clouds,wind,
    if (index.row() >= m_api_output.size() - 1) {
        return QVariant();
    }
    if (m_api_output_lines.at(index.row()).size() < 7) {
        return QVariant();
    }

    switch (role) {
        case TimeRole:
            return QString::fromStdString(
              m_api_output_lines.at(index.row()).at(1)
              );
        case DateRole:
            return QString::fromStdString(
              m_api_output_lines.at(index.row()).at(2)
              );
        case TemperatureRole:
            return QString::fromStdString(
              m_api_output_lines.at(index.row()).at(3)
              );
        case RainRole:
            return get_rain_ammount(m_api_output_lines, index.row());
        case CloudsRole:
            return QString::fromStdString(
              m_api_output_lines.at(index.row()).at(5));
        case WindRole:
            return QString::fromStdString(
                m_api_output_lines.at(index.row()).at(6));
        default:
            return QVariant();
    }
}

void
WeatherListModel::populate()
{
    beginResetModel();
    auto utc_diff = std::to_string(m_utc_diff);
    m_api_output = read_weather(m_weather_program, m_location, utc_diff);
    m_api_output_lines = split_api_output(m_api_output);

    std::string parsestr = m_api_output;

    endResetModel();
}

QHash<int, QByteArray>
WeatherListModel::roleNames() const
{
    QHash<int, QByteArray> result;
    result[TimeRole] = "timestamp";
    result[DateRole] = "date";
    result[TemperatureRole] = "temperature";
    result[RainRole] = "rain";
    result[WindRole] = "wind";
    result[CloudsRole] = "clouds";

    return result;
}
