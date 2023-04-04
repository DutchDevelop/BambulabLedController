#ifndef HTML_H
#define HTML_H

const char* html_setuppage = "\
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