//document.getElementById("datetime").innerHTML = "WebSocket is not connected";

var websocket = new WebSocket('ws://'+location.hostname+'/');
var maxLine = 10;
var currentLine = 0;

function btnClear() {
	console.log('btnClear');
	var element = document.getElementById('reply');
	element.value = "";
}

function btnCopy() {
	console.log('btnCopy');
	var element = document.getElementById('reply');
	var str = element.value;

	if(navigator.clipboard){
		console.log("clipboard.writeText");
		navigator.clipboard.writeText(str);
	} else {
		// https://qiita.com/yamayamasou/items/a53bd71894a6058c5f30
		console.log("clipboard not found");
		const textarea = document.createElement('textarea');
		textarea.value = str;
		document.body.appendChild(textarea);
		textarea.select();
		const result = document.execCommand('copy');
		document.body.removeChild(textarea);
		//alert("This browser is not supported");
	}
}

function getTextValueByName(name) {
	var textbox = document.getElementsByName(name)
	//console.log('name=', name);
	//console.log('textbox=', textbox);
	//console.log('textbox.length=', textbox.length);
	ret = new Array();
	for (var i=0;i<textbox.length;i++) {
		//console.log('textbox[%d].value=%s', i, textbox[i].value);
		ret[i] = textbox[i].value;
	}
	//console.log('typeof(ret)=', typeof(ret));
	//console.log('ret=', ret);
	return ret;
}

function sendText(name) {
	console.log('sendText');
	var data = {};
	data["id"] = 'text';

	var payload_object = getTextValueByName(name);
	console.log('payload_object=' + payload_object);
	console.log('type payload=' + (typeof payload_object));
	var payload_str = payload_object.toString();
	console.log('type payload_str=' + (typeof payload_str));
	data["data"] = payload_str;
	console.log('data=', data);
	json_data = JSON.stringify(data);
	console.log('json_data=' + json_data);
	websocket.send(json_data);
}

function sendId(id) {
	console.log('sendId id=[%s]', id);
	var data = {};
	data["id"] = id;
	console.log('data=', data);
	json_data = JSON.stringify(data);
	console.log('json_data=' + json_data);
	websocket.send(json_data);
}

function sendIdValue(id, value) {
	console.log('sendIdValue id=[%s] value=[%s]', id, value);
	var data = {};
	data["id"] = id;
	data["value"] = value;
	console.log('data=', data);
	json_data = JSON.stringify(data);
	console.log('json_data=' + json_data);
	websocket.send(json_data);
}

websocket.onopen = function(evt) {
	console.log('WebSocket connection opened');
	var data = {};
	data["id"] = "init";
	console.log('data=', data);
	json_data = JSON.stringify(data);
	console.log('json_data=' + json_data);
	websocket.send(json_data);
	//document.getElementById("datetime").innerHTML = "WebSocket is connected!";
}

websocket.onmessage = function(evt) {
	var msg = evt.data;
	console.log("msg=" + msg);
	var values = msg.split('\4'); // \4 is EOT
	console.log("values=" + values);
	switch(values[0]) {
		case 'REPLY':
			//console.log("values[1]=" + values[1]);
			var element = document.getElementById('reply');
			var value = element.value;
			value = value.trim();
			console.log("value=[%s]", value);
			console.log("value.length=[%d]", value.length);
			element.value = value + "\n" + values[1];
			break;

		default:
			break;
	}
}

websocket.onclose = function(evt) {
	console.log('Websocket connection closed');
	//document.getElementById("datetime").innerHTML = "WebSocket closed";
}

websocket.onerror = function(evt) {
	console.log('Websocket error: ' + evt);
	//document.getElementById("datetime").innerHTML = "WebSocket error????!!!1!!";
}
