from decode import Weather
import requests
def to_command(weather):
    base = "weather,location=ETSIINF"
    command = '''{base} temperature={temp}
{base} pressure={pres}
{base} humidity={hum}
{base} wind_direction={windd}
{base} wind_speed={winds}
{base} precipitation={prec}
'''.format(
    base=base,
    temp=weather.temp,
    pres=weather.pres,
    hum=weather.hum,
    windd=Weather.wind_dirs[weather.windD],
    winds=weather.windS,prec=weather.prec
)
    return command

def send_data(weather):
    url = "http://jupiter.datsi.fi.upm.es:8086/write"
    params={"db":"weather"}
    body = to_command(weather)
    resp = requests.post(url,params=params,data=body)
