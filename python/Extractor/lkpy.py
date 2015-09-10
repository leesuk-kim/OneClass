
__author__ = 'lk'


class LkIo:
    import mysql.connector as connector

    def __init__(self):
        self.host = '221.153.219.220'
        self.port = '8806'
        self.user = 'niplab'
        self.password = 'qwqw`12'
        self.database = 'LIG'
        self.connect = LkIo.connector.connect(host=self.host, port=self.port, user=self.user,
                                              password=self.password, database=self.database)
        self.cursor = self.connect.cursor()

    def __del__(self):
        self.cursor.close()
        self.connect.close()

    def local_import(self):
        pass

    def mysql_init(self, host, port, user, password, database):
        self.host = host
        self.port = port
        self.user = user
        self.password = password
        self.database = database
        pass

    def mysql_export_inputdata(self, tbname, data):
        cursor = self.cursor
        # TODO data의 열개수 읽어서 뒷부분 완성하기
        cursor.execute("DROP TABLE IF EXISTS {}".format(tbname))
        cursor.execute("CREATE TABLE " + tbname +
                       "\'target\' VARCHAR(10) NOT NULL PRIMARY KEY,"
                       "\'\'")

        pass
    pass
