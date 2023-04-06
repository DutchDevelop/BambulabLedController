#ifndef HTML_H
#define HTML_H

const char* html_setuppage = "\
<!DOCTYPE html>\
<head>\
    <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css' integrity='sha512-ixlOZJxl8aj1Cz0mKjyFTJQx/s+U6wo0o6P+CZPRJX+gze3Jh8Fro/mTyLr5r/Vx+uV7J8RvRfZp5+X9fivG7A==' crossorigin='anonymous' referrerpolicy='no-referrer' />\
    <style>\
        body {\
            background-color: #f1f1f1;\
            font-family: Arial, sans-serif;\
        }\
        .container {\
            display: flex;\
            justify-content: center;\
            align-items: center;\
            height: 100vh;\
        }\
        .form-container {\
            background-color: white;\
            padding: 20px;\
            border-radius: 10px;\
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.3);\
            width: 50%;\
        }\
        .upload-button {\
            position: absolute;\
            top: 10px;\
            right: 10px;\
            cursor: pointer;\
            color: #ccc;\
            font-size: 20px;\
        }\
        .upload-button:hover {\
            color: #333;\
        }\
        label {\
            display: block;\
            width: 100%;\
            margin-bottom: 5px;\
            font-weight: bold;\
            font-size: 16px;\
            color: #333;\
        }\
        input[type='text'] {\
            width: 100%;\
            padding: 10px;\
            border-radius: 5px;\
            border: 1px solid #ccc;\
            margin-bottom: 20px;\
            font-size: 16px;\
        }\
        input[type='submit'] {\
            background-color: #4CAF50;\
            color: white;\
            padding: 10px;\
            border-radius: 5px;\
            border: none;\
            cursor: pointer;\
            margin-top: 20px;\
            font-size: 16px;\
            font-weight: bold;\
        }\
        form {\
            text-align: left;\
        }\
    </style>\
</head>\
<body>\
    <div class='container'>\
        <div class='form-container'>\
            <h1>BL LED Controller Setup Page</h1>\
            <p>\
                This page allows you to set up your BL LED controller by configuring your MQTT broker, access code, and serial ID.\
            </p>\
            <form method='POST' action='/setupmqtt'>\
                <label>IP:</label>\
                <input type='text' name='ip' title='Enter the IP address of your MQTT broker' value='ipinputvalue'><br>\
                <label>Access Code:</label>\
                <input type='text' name='code' title='Enter the access code for your MQTT broker' value='codeinputvalue'><br>\
                <label>Serial ID:</label>\
                <input type='text' name='id' title='Enter the serial ID for your device' value='idinputvalue'><br>\
                <input type='submit' value='Save'>\
            </form>\
            <label>Firmware Update:</label>\
            <form method='POST' enctype='multipart/form-data' id='upload_form'>\
                <input accept='.bin' class='input-file' id='file1' name='firmware' type='file'><br>\
                <h3 id='uploadstatus'></h3>\
                <input type='submit' value='Update'>\
            </form>\
            <script type='application/javascript'>\
                function _(selector) {\
                    return document.querySelector(selector);\
                }\
                var uploadForm = _('#upload_form');\
                console.log(uploadForm);\
                uploadForm.addEventListener('submit', function(event) {\
                    event.preventDefault();\
                    uploadFile();\
                });\
                function uploadFile() {\
                    var fileInput = _('#file1');\
                    console.log(fileInput);\
                    if (!fileInput || !fileInput.files || fileInput.files.length === 0) {\
                        console.error('No file selected');\
                        return;\
                    }\
                    var file = fileInput.files[0];\
                    console.log(file);\
                    var formdata = new FormData();\
                    formdata.append('firmware', file, file.name);\
                    var ajax = new XMLHttpRequest();\
                    ajax.upload.addEventListener('progress', progressHandler, false);\
                    ajax.open('POST', '/update');\
                    ajax.setRequestHeader('Access-Control-Allow-Headers', '*');\
                    ajax.setRequestHeader('Access-Control-Allow-Origin', '*');\
                    ajax.addEventListener('load', completeHandler, false);\
                    ajax.send(formdata);\
                };\
                function progressHandler(event) {\
                    var percent = Math.round((event.loaded / event.total) * 100);\
                    _('#uploadstatus').innerHTML = 'Uploading. ' + percent + '%';\
                }\
                function completeHandler(event) {\
                    if (event.target.responseText.indexOf('error') >= 0) {\
                        _('#status').innerHTML = event.target.responseText;\
                    } else {\
                        _('#status').innerHTML = 'Upload Success!';\
                    }\
                }\
            </script>\
        </div>\
    </div>\
</body>";

const char* html_finishpage = "\
<!DOCTYPE html>\
<html>\
<head>\
	<title>BL LED Controller Setup</title>\
	<style>\
		body {\
			background-color: #f1f1f1;\
			font-family: Arial, Helvetica, sans-serif;\
		}\
		.container {\
			background-color: #ffffff;\
			border-radius: 10px;\
			box-shadow: 0px 0px 10px 5px rgba(0,0,0,0.2);\
			padding: 20px;\
			margin-top: 50px;\
			margin-left: auto;\
			margin-right: auto;\
			max-width: 500px;\
			text-align: center;\
		}\
		h1 {\
			font-size: 36px;\
			margin-bottom: 20px;\
		}\
		p {\
			font-size: 18px;\
			margin-bottom: 20px;\
		}\
		button {\
			background-color: #4CAF50;\
			color: white;\
			padding: 10px;\
			border-radius: 5px;\
			border: none;\
			cursor: pointer;\
			margin-top: 20px;\
			font-size: 18px;\
		}\
		button:hover {\
			background-color: #3e8e41;\
		}\
	</style>\
</head>\
<body>\
	<div class='container'>\
		<h1>Successfully saved parameters</h1>\
		<p>Your parameters have been saved. You can now close this window and return to the main menu.</p>\
	</div>\
</body>\
</html>";

#endif