Для работы серверной части нужнен MySQL / PostgreSQL и odbc driver для вашего SQL.

<h2>Инструкция сервера:</h2>

1. Запустить сервер
2. Сервер предложит вам конфиг подключения в фотрмате:
    ```
    DRIVER={MySQL ODBC 9.2 ANSI Driver};
    SERVER=localhost;
    PORT=3306;
    DATABASE=your_dbname;
    UID=your_username;
    PWD=your_passsword;
    ```
    {MySQL ODBC 9.2 ANSI Driver} - У меня такой драйвер, пишите ваш если у вас другой.<br>
    your_dbname - ваше имя базы данных (если первый раз, то ваше будушее имя базы данных)<br>
    your_username - ваш логин<br>
    your_passsword - ваш пароль<br>
    <br />
    Если поставить галку "Save config" то сервер сохранит ваш конфиг в файл dbconnection.conf рядом<br>
    с .exe файлом. При повторном запуске конфиг грузится из файла.<br>
    <br />
    Если поставить галку "Create new database" то будет создана новая база данных с именем которые<br>
    вы указали на месте your_dbname.<br>
    <br />
3. Нажать "Connect Database" если все получилось, то увидите зеленое сообщение.<br>
   Если нет, то увидите лог ошибки.
4. Нажать "Start server" если все получилось, то увидите зеленое сообщение.<br>
   Если нет, то увидите лог ошибки.
5. Нажать "To control panel" если вы видели 2 зеленых сообщения то откроется контнольная панель<br>

<h4>ЭЛЕМЕНТЫ КОНТРОЛЬНОЙ ПАНЕЛИ</h4>

1. Слева логи того, что будет происходить на сервере.
2. Справва кнопка "Listen for new client connection" - сервер начнет ждать входящее подключение ТОЛЬКО ПОСЛЕ НАЖАТИЯ этой кнопки.
   Лампочка в правом верхнем углу - индикатор подключения.<br>
   <br />
   Красный - пользователь не подключен<br>
   Желтый - ждет подключения (по умолчанию таймаут 25 секунд)<br>
   Зеленый - пользователь подключился<br>
   <br />
3. Ниже поле поиска пользователя, пишем ник и жмем "Find" - загорится лампочка зеленый\красный нашли\нет.<br>
4. Пользователя которого нашли можно забанить до рестарта сервера по кнопке "Ban untill server restart".<br>
5. "Show banned users" - поспотреть забаненых пользователей.<br>
6. "Show all users" - поспотреть всех пользователей.<br>

<h5>Дополнения по заботе сервера</h5>
1. Всё что ниже строки "User control panel" работает и без подключения клиента<br>
2. После того как клиент закрыл свою программу лампочка в верхнем правом углу станет красной.<br>
3. Чтобы подлючиться к серверу еще просто жмите "Listen for new client connection", перезапуск не требуется!<br>
4. Конфиг подключения в датабазе жуть какой чувствительный к пробелам, я постарался исправить это в программе, должно работать даже если где то будет лишний пробел.

<h2>Инструкция клиента:</h2>

1. Нажать на сервере "Listen for new client connection".<br>
2. Запусить клиент.<br>
3. Залогиниться или создать нового пользователя.<br>
4. Вкладки приватных и общих сообщений можно переключить в меню баре. "Menu" сверху слева.<br>

<h5>Общие сообщения</h5>
1. Все сообщения у левого края.<br>
2. У ваших сообщений другой фон.<br>

<h5>Приватные сообщения</h5>
1. Отправленные сообщения у правого края.<br>
2. У отправленных сообщений в заголовке имя получателя.<br>
3. Полученные сообщения у левого края.<br>
4. У полученных сообщений в заголовке имя отправителя.<br>
5. У непрочитанных сообщений другой фон.<br>

<h5>Дополнение</h5>
Обновление списка сообщений происходит только при смене вкладки

