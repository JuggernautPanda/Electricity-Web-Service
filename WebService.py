#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  untitled.py
#  
#  Copyright 2018 raja <raja@raja-Inspiron-N5110>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  https://github.com/marcoschwartz/aREST
#  https://www.elastic.co/guide/en/kibana/current/console-kibana.html
#
#  Read about updating a document in ES : https://www.elastic.co/guide/en/elasticsearch/guide/current/update-doc.html

"""

GET /_cat/indices?v

PUT /twitter?pretty
 { "settings" : { "index" : { "number_of_shards" : 3, "number_of_replicas" : 2 } } }
 
DELETE twitter

"""


import requests
import json

def CheckIndex():
	response = requests.get('http://x.x.x.x:9200/electric/?pretty')
	print (response.content)

def CheckData():
	response = requests.get('http://x.x.x.x:9200/electric/_search?pretty=true&size=110&scroll=10m&q=*:*')
	print (response.content)

def DeleteIndex():
	response = requests.delete('http://x.x.x.x:9200/electric/')
	print (response.content)

def Method2():
	data = '\n{\n    "query" : {\n        "range": { "_timestamp": { "gt": "now-20H"}\n    }\n }}'
	headers = {'content-type': 'application/json'}
	response = requests.get('http://x.x.x.x:9200/electric/_search?pretty=true&scroll=10m&q=*:*', data=data, headers=headers)
	print (res)
	#"localhost:9200/twitter?pretty' -H 'Content-Type: application/json' -d' { "settings" : { "index" : { "number_of_shards" : 3, "number_of_replicas" : 2 } } } "

def PushTheData():
	print ("Hi")


def CreateTheIndex():
	data = {
    "mappings": {
        "reading": {
            "properties": {
                "Voltage": {"type": "float"},
                "Current": {"type": "float"},
                "AmpereHour": {"type": "float"},
                "timestamp": {"type": "date"}
            },
        }
    }
}
	headers = {'content-type': 'application/json'}
	response = requests.put('http://localhost:9200/electric', data=json.dumps(data), headers=headers)
	try:
		assert response.status_code is 200
		print ("Successfully created the elasticsearch index")
	except AssertionError:
		raise AssertionError("Your mapping was not created", response.content)
	# You could use this snippet to delete your old data if you have an error.
	#response = requests.delete('http://localhost:9200/electric')
	#try:
	#    assert response.status_code is 200
	#except AssertionError:
	#    raise AssertionError("Your mapping was not deleted", response)

def main(args):
	#CreateTheIndex()
	CheckIndex()
	CheckData()
	#DeleteIndex()

	return 0

if __name__ == '__main__':
    import sys
    sys.exit(main(sys.argv))
