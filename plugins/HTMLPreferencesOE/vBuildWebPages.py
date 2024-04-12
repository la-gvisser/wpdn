import os

def do_page(page):
    wd = os.getcwd()
    os.chdir(page)
    base_title = 'HTML Preferences OE'
    base_file = "%s/index.html" % (page)
    for i in range(1, 5):
        sub_file = "%s-%d/index.html" % (page, i)
        sub_title = "HTML Prefs OE - %d" % (i)
        with open(base_file, 'r') as base, open(sub_file, 'w') as sub:
            for line in base:
                sub.write(line.replace(base_title, sub_title))
    os.chdir(wd)

old_wd = os.getcwd()
os.chdir('./HTMLPreferencesOE/Plugin')

do_page('About')
do_page('CaptureTab')
do_page('EngineTab')
do_page('Options')

os.chdir(old_wd)
