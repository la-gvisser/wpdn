import os
import sys
import locale
import io
import getopt
import time
import geoip2.database

# Savvius.com 184.105.70.163

def to_int(ip):
    t = ip.split('.')
    return (int(t[0]) << 24) + (int(t[1]) << 16) + (int(t[2]) << 8) + int(t[3])


def format_ip(ip):
    return '%d.%d.%d.%d' % (ip >> 24,(ip >> 16) & 0x0FF, (ip >> 8) & 0x0FF, ip & 0x0FF)


class CountryFileGenerator:
    """Country File Generator"""

    db_name = "maxmind-db.mmdb"
    """Default databse name."""

    file_name = "_co_code_names.txt"
    """Default file name."""

    first = 0
    """First IP address to check."""

    last = 0xFFFFFFFF
    """Last IP address to check."""

    delta = 256
    """The increment value of the next IP address."""

    option_ip = False
    """Add a trailing colon and IP address."""

    option_all_ip = False
    """Add a trailing colon and all IP addresses."""

    def __init__(self):
        self.db_name = CountryFileGenerator.db_name
        self.file_name = CountryFileGenerator.file_name
        self.first = CountryFileGenerator.first
        self.last = CountryFileGenerator.last
        self.delta = CountryFileGenerator.delta
        self.option_ip = CountryFileGenerator.option_ip
        self.option_all_ip = CountryFileGenerator.option_all_ip

    def generate(self):
        code_names = {}
        reader = geoip2.database.Reader(self.db_name)

        ip = self.first
        prev_code = 0

        start_time = time.time()
        print time.asctime(time.localtime(start_time))

        while ip <= self.last:
            try:
                cr = reader.city(format_ip(ip))
                if cr.country.geoname_id not in code_names.keys():
                    code_names[cr.country.geoname_id] = (cr.country.iso_code, cr.country.name, format_ip(ip))
                    record = '%s\t%s' % (cr.country.iso_code, cr.country.name)
                    if self.option_ip:
                        record += ' : %s' % format_ip(ip)
                    print record
                    prev_code = cr.country.geoname_id
                elif self.option_all_ip and prev_code != cr.country.geoname_id:
                    tup = code_names[cr.country.geoname_id]
                    code_names[cr.country.geoname_id] = (tup[0], tup[1], tup[2] + ' %s' % format_ip(ip))
                    prev_code = cr.country.geoname_id
            except:
                pass
            ip += self.delta

        end_time = time.time()
        print time.asctime(time.localtime(end_time))
        print '%f Seconds' % (end_time - start_time)

        lst = code_names.values()
        lst.sort(key=lambda x: x[0])

        line_count = 0
        with io.open(self.file_name, 'w', encoding=locale.getpreferredencoding()) as ft:
            for item in lst:
                line = '%s\t%s' % (item[0], item[1])
                if self.option_ip:
                    line += ' : %s' % item[2]
                line += '\n'
                try:
                    ft.write(line)
                except:
                    print('Exception writing line: %d' % line_count)
                line_count += 1
        
        return code_names

    def parse(self):
        code_names = {}
        with io.open(self.file_name, 'r', encoding=locale.getpreferredencoding()) as ft:
            for line in ft:
                if len(line) > 0:
                    if ':' in line:
                        t, ip = line.split(':', 1)
                    else:
                        t = line
                    iso, name = t.strip().split(' ', 1)
                    if iso not in code_names.keys():
                        code_names[iso] = name.strip()
        return code_names

    def display_help(self):
        print('generate-country-codes-geoip2.py')
        print('  -d : name and path of the database file. Default is maxmind-db.mmdb.')
        print('  -f : name of the file to create. Default is _co_code_names.txt.')
        print('  -i : add a trailing colon `:` and IP address to the output.')
        print('  -I : same as -i but with all IP addresses.')
        print('  -t : CIDR for a test run. Default is no test run.')

    def parse_args(self, argList):
        try:
            (opts, args) = getopt.getopt(argList, "iId:f:t:?")
        except getopt.GetoptError as error:
            # print(help information and exit:)
            self.display_help()
            sys.exit(2)

        for opt, arg in opts:
            if opt in ("-d"):
                self.db_name = arg
            elif opt in ("-f"):
                self.file_name = arg
            elif opt in ("-i"):
                self.option_ip = True
            elif opt in ("-I"):
                self.option_ip = True
                self.option_all_ip = True
            elif opt in ("-t"):
                ip, bits = arg.split("/")
                self.first = to_int(ip)
                mask = int(bits)
                if mask > 31 or mask < 1:
                    print 'Invalid CIDR: ', arg
                    self.display_help()
                    sys.exit(2)
                rng = 2 << (31 - mask)
                self.last = self.first + rng

def main(args):
    cfg = CountryFileGenerator()
    cfg.parse_args(args)
    cng = cfg.generate()
    cnp = cfg.parse()


if __name__ == '__main__':
    args = sys.argv[1:]
    main(args)
