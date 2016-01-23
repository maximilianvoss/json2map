# Jsontable
Transforming JSON into a hashmap for easier access and processing in C.

Starting with a JSON object, looking like:

```
{  
   "test":{  
      "_id":{  
         "$oid":"566950d1afc4a3c1d86fcdfb"
      },
      "name":"picture",
      "file":"/var/www/html/pictureIn.png",
      "array":[  
         "1",
         "b",
         "3",
         "d",
         {  
            "object":"test"
         }
      ],
      "nullpointer":null,
      "number":1234,
      "true":true,
      "false":false
   }
}
```

Will be turned into:
```
test._id.$oid = 566950d1afc4a3c1d86fcdfb
test.name = picture
test.file = /var/www/html/pictureIn.png
test.array[0] = 1
test.array[1] = b
test.array[2] = 3
test.array[3] = d
test.array[4].object = test
test.nullpointer = null
test.number = 1234
test.true = true
test.false = false
```