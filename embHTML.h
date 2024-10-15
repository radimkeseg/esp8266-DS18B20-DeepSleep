/**The MIT License (MIT)

Copyright (c) 2019 by Radim Keseg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Arduino.h" //not sure why but here it is needed

const char PAGE_INDEX[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Digi Temp v4</title>


  <style>
.rGauge {
  width: 200px;
  height: 145px;
}
.rGauge-base {
  stroke: #edebeb;
  fill: none;
}
.rGauge-progress {
  fill: none;
  transition: 0.5s;
}
.rGauge-val {
  font-weight: 700;
  font-size: 1.3em;
}
.rGauge-min-val, .rGauge-max-val {
  fill: #b3b3b3;
}
.vGauge {
  width: 145px;
  height: 145px;
}
.vGauge-base {
  fill: #edebeb;
}
.vGauge-progress {
  transition: 0.5s;
}
.vGauge-val {
  font-weight: 700;
  font-size: 1.3em;
}
.vGauge-min-val, .vGauge-max-val {
  fill: #b3b3b3;
} 
  </style>
  <style>
body {
  margin: 0;
  padding: 20px;
  font-family: Helvetica,Arial,sans-serif;
  font-size: 14px;
  line-height: 1.42857143;
  color: #333;
  background-color: #fff;
}
.gauge-section {
  display: inline-block;
  text-align: center;
}
.gauge-section h3 {
  margin-bottom: 0;
}
.gauge {
  display: inline-block;
}
.actions {
  margin-top: 15px;
}
.btn {
  color: #333;
  border: 0;
  border-radius: 3px;
  box-shadow: 0px 1px 3px #666666;
  background: #fff;
  padding: 10px 20px 10px 20px;
  text-decoration: none;
  cursor: pointer;
}
.btn-icon {
  padding: 2px 15px 2px 15px;
}
.btn:active {
  background: #dddddd;
  text-decoration: none;
} 

</style>
 
<style>
hr {
  border: 0px;
  border-bottom: 1px solid black;
}
.btn-group {
  border: 0px solid black;
  -webkit-border-radius: 0px 10px 10px 10px;
  border-radius: 0px 10px 10px 10px; 
  background-color: #feefc3;
  padding: 5px;
  margin-bottom: 20px;
}
checkbox input:before {
  content: '';
  background: #fff;
  display: block;
  width: 10px;
  height: 10px;
  border: 1px solid black;
  margin-left: 0px;
}

input{
  transition: 0.5s;
}

label{
  display: block;
  width:100px;
  height: 25px;
  display: table-cell;
  vertical-align: bottom;
  white-space: nowrap;
}
label.title{
  width:150px;
  padding-left: 10px;
  padding-right: 10px;
  border-radius: 10px 10px 0px 0px; 
  background-color: #feefc3;
}
 </style>
 
 <style>
.password + .unmask {
  position: realtive;
  right: 10px;
  top: 50%;
  transform: translateY(0%);
  text-indent: -9999px;
  width: 25px;
  height: 25px;
  background: #ccc;
  border-radius: 50%;
  cursor:pointer;
  border: none;
  -webkit-appearance:none;
}

.password + .unmask:before {
  content: "";
  position: absolute;
  top: 5px;
  left: 5px;
  width: 15px;
  height: 15px;
  background: #e3e3e3;
  z-index:1;
  border-radius: 50%;
}

.password[type="text"] + .unmask:after {
  content: "";
  position: absolute;
  top: 7px;
  left: 7px;
  width: 11px;
  height: 11px;
  background: #ccc;
  z-index: 2;
  border-radius: 50%;
}

 </style>
</head>

<body>
<h1>DigiTemp v4</h1>
<p style='text-align: right'>by Radim Keseg</p>

<form method='post' action='setup'>

<label class='title'><input id='_gs' name='_gs' type='checkbox' {gs}>Measure and Send Periodically</label>
<div class='btn-group'>
<table>
<tr><td><label>Update interval (s)</label></td><td><input id='_gs_update_interval' name='_gs_update_interval' length=4 pattern='^[0-9]{1,16}$' required value='{gs_update_interval}'></td></tr>
<tr><td><label><input id='_gs_deep_sleep' name='_gs_deep_sleep' type='checkbox' {gs_deep_sleep}>Deep Sleep</label></td></tr>
</table>
</div>

<label class='title'><input id='_ts' name='_ts' type='checkbox' {ts}>Send to ThingSpeak</label>
<div class='btn-group'>
<table>
<tr><td><label>Channel</label></td><td><input id='_ts_channel' name='_ts_channel' length=16 pattern='^[0-9]{1,16}$' required value='{ts_channel}'></td></tr>
<tr><td><label>Write API Key</label></td><td><input id='_ts_write_api_key' name='_ts_write_api_key' length=16 pattern='^[0-9a-zA-Z]{16}$' required value='{ts_write_api_key}'></td></tr>
<tr><td><label>Temperature Field #</label></td><td><input id='_ts_field_temp' name='_ts_field_temp' length=1 pattern='^[0-8]{1}$' required value='{ts_field_temp}'></td></tr>
</table>
</div>

<label class='title'><input id='_mqtt' name='_mqtt' type='checkbox' {mqtt}>Send to Home Assisstant / MQTT broker</label>
<div class='btn-group'>
<table>
<tr><td><label>MQTT broker</label></td><td><input id='_mqtt_broker' name='_mqtt_broker' length=16 pattern='^.{1,255}$' required value='{mqtt_broker}'></td></tr>
<tr><td><label>User</label></td><td><input id='_mqtt_user' name='_mqtt_user' length=16 pattern='^[0-9a-zA-Z_\.]{1,255}$' required value='{mqtt_user}'></td></tr>
<tr><td><label>Password</label></td><td><input type='password' id='_mqtt_password' name='_mqtt_password' class='password' length=16 pattern='^.{1,255}$' required value='{mqtt_password}'><button class="unmask" type="button" title="Mask/Unmask password to check content" onClick="unmask('_mqtt_password')">Unmask</button></td></tr>
<tr><td><label>DeviceID</label></td><td><input id='_mqtt_device_id' name='_mqtt_device_id' length=16 pattern='^[0-9a-z_]{1,255}$' required value='{mqtt_device_id}'></td></tr>
<tr><td><label>Out Topic</label></td><td><input id='_mqtt_otopic' name='_mqtt_otopic' length=32 pattern='^[0-9a-z_/]{1,255}$' required value='{mqtt_otopic}'></td></tr>
<tr><td><label>In Topic</label></td><td><input id='_mqtt_itopic' name='_mqtt_itopic' length=32 pattern='^[0-9a-z_/]{1,255}$' required value='{mqtt_itopic}'></td></tr>
</table>
</div>




<input type='submit' value='Store' class='btn'></form>

<hr/>

  <div>
    <div class="gauge-section">
      <h3>Temperature</h3>
      <div id="temp" class="gauge"></div>
    </div>
  </div>
  <div class="actions">
    <button id="refresh" class="btn btn-icon">
      <svg height="30" viewBox="0 0 24 24" width="30">
          <path d="M17.65 6.35C16.2 4.9 14.21 4 12 4c-4.42 0-7.99 3.58-7.99 8s3.57 8 7.99 8c3.73 0 6.84-2.55 7.73-6h-2.08c-.82 2.33-3.04 4-5.65 4-3.31 0-6-2.69-6-6s2.69-6 6-6c1.66 0 3.14.69 4.22 1.78L13 11h7V4l-2.35 2.35z"/>
          <path d="M0 0h24v24H0z" fill="none"/>
      </svg>
    </button>
  </div>
  <script>
/*
  Switch actions
*/
function unmask(pass_id){
  pass = document.getElementById(pass_id);
  if(pass.getAttribute('type') == 'password')
    pass.setAttribute('type','text');
  else
    pass.setAttribute('type','password');
  return false;
}  
  </script>

  <script>
function createRadGauge(id, minVal, maxVal, unit) {
  function polarToCartesian(centerX, centerY, radius, rad) {
    return {
      x: centerX + (radius * Math.cos(rad)),
      y: centerY + (radius * Math.sin(rad))
    };
  }
  
  function arc(x, y, radius, val, minVal, maxVal){
      var start = polarToCartesian(x, y, radius, -Math.PI);
      var end = polarToCartesian(x, y, radius, -Math.PI*(1 - 1/(maxVal-minVal) * (val-minVal)));
  
      var d = [
          "M", start.x, start.y, 
          "A", radius, radius, 0, 0, 1, end.x, end.y
      ].join(" ");
  
      return d;       
  }

  var tmpl = 
  '<svg class="rGauge" viewBox="0 0 200 145">'+ 
    '<path class="rGauge-base" id="'+id+'_base" stroke-width="30" />'+ 
    '<path class="rGauge-progress" id="'+id+'_progress" stroke-width="30" stroke="#1565c0" />'+ 
    '<text class="rGauge-val" id="'+id+'_val" x="100" y="105" text-anchor="middle"></text>'+  
    '<text class="rGauge-min-val" id="'+id+'_minVal" x="40" y="125" text-anchor="middle"></text>'+  
    '<text class="rGauge-max-val" id="'+id+'_maxVal" x="160" y="125" text-anchor="middle"></text>'+  
  '</svg>';

  document.getElementById(id).innerHTML = tmpl;
  document.getElementById(id+'_base').setAttribute("d", arc(100, 100, 60, 1, 0, 1));
  document.getElementById(id+'_progress').setAttribute("d", arc(100, 100, 60, minVal, minVal, maxVal));
  document.getElementById(id+'_minVal').textContent = minVal;
  document.getElementById(id+'_maxVal').textContent = maxVal;

  var gauge = {
    setVal: function(val) {
      val = Math.max(minVal, Math.min(val, maxVal));
      document.getElementById(id+'_progress').setAttribute("d", arc(100, 100, 60, val, minVal, maxVal));
      document.getElementById(id+'_val').textContent = val + (unit !== undefined ? unit: '');
      return gauge;
    },
    setColor: function(color) {
       document.getElementById(id+'_progress').setAttribute("stroke", color);
       return gauge;
    }
  }
  
  return gauge;
}

function createVerGauge(id, minVal, maxVal, unit) {
  var tmpl = 
  '<svg class="vGauge" viewBox="0 0 145 145">'+
    '<rect class="vGauge-base" id="'+id+'_base" x="30" y="25" width="30" height="100"></rect>'+
    '<rect class="vGauge-progress" id="'+id+'_progress" x="30" y="25" width="30" height="0" fill="#1565c0"></rect>'+
    '<text class="vGauge-val" id="'+id+'_val" x="70" y="80" text-anchor="start"></text>'+
    '<text class="vGauge-min-val" id="'+id+'_minVal" x="70" y="125"></text>'+
    '<text class="vGauge-max-val" id="'+id+'_maxVal" x="70" y="30" text-anchor="start"></text>'+
  '</svg>';
  
  document.getElementById(id).innerHTML = tmpl;
  document.getElementById(id+'_minVal').textContent = minVal;
  document.getElementById(id+'_maxVal').textContent = maxVal;
  
  var gauge = {
    setVal: function(val) {
      val = Math.max(minVal, Math.min(val, maxVal));
      var height = 100/(maxVal-minVal) * (val-minVal);
      
      document.getElementById(id+'_progress').setAttribute("height", height);
      document.getElementById(id+'_progress').setAttribute("y", 25+(100-height));
      document.getElementById(id+'_val').textContent = val + (unit !== undefined ? unit: '');
      return gauge;
    },
    setColor: function(color) {
       document.getElementById(id+'_progress').setAttribute("fill", color);
       return gauge;
    }
  }
  
  return gauge;
}     
  </script>
  <script>
function getTempColor(t) {
  if (t >= 35) {
    return '#ff5722';
  } else if (t >= 30) {
    return '#ff9800';
  } else if (t >= 25) {
    return '#ffc107';
  } else if (t >= 18) {
    return '#4caf50';
  } else if (t > 10) {
    return '#8bc34a';
  } else if (t >= 5) {
    return '#00bcd4';
  } else if (t >= -5) {
    return '#03a9f4';
  } else {
    return '#2196f3';
  }
}

function getMoisColor(x) {
  var colors = ['#E3F2FD','#BBDEFB','#90CAF9','#64B5F6','#42A5F5','#2196F3','#1E88E5','#1976D2','#1565C0','#0D47A1','#0D47A1'];
  return colors[Math.round(x/10)];
}

function refresh() {
  var xmlHttp = new XMLHttpRequest();

  xmlHttp.onreadystatechange = function()
  {
    if (xmlHttp.readyState == XMLHttpRequest.DONE) {
        if (xmlHttp.status == 200)
        {
          var data = JSON.parse(xmlHttp.responseText);
          tempGauge.setVal(data.temp).setColor(getTempColor(data.temp));
        } else {
          console.log('Refresh failed: ' + xmlHttp.status);
        }
    }
  }

  xmlHttp.open("GET", "data", true);
  xmlHttp.send();
}

var tempGauge = createVerGauge('temp', -20, 60, '°C').setVal(0).setColor(getTempColor(0));

function gs_checkbox(){
  var checkBox = document.getElementById('_gs');  
  document.getElementById("_gs_update_interval").disabled  = !checkBox.checked;
  document.getElementById("_gs_deep_sleep").disabled  = !checkBox.checked;
  document.getElementById('_ts').disabled  = !checkBox.checked;
  document.getElementById('_mqtt').disabled  = !checkBox.checked;  
}

function ts_checkbox(){
  var checkBox = document.getElementById('_ts');  
  document.getElementById("_ts_channel").disabled  = !checkBox.checked;
  document.getElementById("_ts_write_api_key").disabled  = !checkBox.checked;
  document.getElementById("_ts_field_temp").disabled  = !checkBox.checked;
}

function mqtt_checkbox(){
  var checkBox = document.getElementById('_mqtt');  
  document.getElementById("_mqtt_broker").disabled  = !checkBox.checked;
  document.getElementById("_mqtt_user").disabled  = !checkBox.checked;
  document.getElementById("_mqtt_password").disabled  = !checkBox.checked;
  document.getElementById("_mqtt_device_id").disabled  = !checkBox.checked;
  document.getElementById("_mqtt_otopic").disabled  = !checkBox.checked;
  document.getElementById("_mqtt_itopic").disabled  = !checkBox.checked;
}

function onLoad(){
  gs_checkbox();
  ts_checkbox();
  mqtt_checkbox();
  refresh();
}

document.getElementById('refresh').addEventListener('click', refresh);
document.getElementById('_gs').addEventListener('click',gs_checkbox);
document.getElementById('_ts').addEventListener('click',ts_checkbox);
document.getElementById('_mqtt').addEventListener('click',mqtt_checkbox);
setTimeout(onLoad, 500);   

//document.addEventListener('load', function(){ onLoad(); } );
  </script>


</body>
</html>
)=====";
