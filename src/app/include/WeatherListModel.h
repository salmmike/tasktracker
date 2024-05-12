#ifndef WEATHERLISTMODEL_H
#define WEATHERLISTMODEL_H
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *
 * Author: Mike Salmela
 */

#include <QAbstractListModel>
#include <QDate>
#include <QTimer>
#include <scheduler.h>

enum WeatherListRole
{
    TimeRole = Qt::UserRole + 1,
    DateRole,
    TemperatureRole,
    RainRole,
    WindRole,
    CloudsRole,
};

class WeatherListModel : public QAbstractListModel
{
    Q_OBJECT

  public:
    explicit WeatherListModel(std::filesystem::path weather_csv_program,
                              std::string location = "nokia",
                              int utc_diff = 3,
                              QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    void populate();

    QHash<int, QByteArray> roleNames() const override;

  private:
    QTimer* m_timer;
    std::filesystem::path m_weather_program;
    std::string m_location;
    int m_utc_diff;
    int m_refresh_time_s{ 3600 };
    std::string m_api_output;
    std::vector<std::vector<std::string>> m_api_output_lines;
};

#endif /* DEVICELISTMODEL_H */
