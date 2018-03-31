function izquierdo(x, y){
    new Ajax.Request('/izquierda', { 
    method: 'post',
    parameters: $H({'csrfmiddlewaretoken':Cookies.get('csrftoken'), 'x':x, 'y':y}),
    });
}
function derecho(x, y){
    new Ajax.Request('/derecha', { 
    method: 'post',
    parameters: $H({'csrfmiddlewaretoken':Cookies.get('csrftoken'), 'x':x, 'y':y}),
    });
}
function overaxis(){
    new Ajax.Request('/sobreEje', {method: 'get'});
}
function openGripper(){
    new Ajax.Request('/open', {method: 'get'});
}
function closeGripper(){
    new Ajax.Request('/close', {method: 'get'});
}
function apagarMotorRiel(){
    new Ajax.Request('/apagarRiel', {method: 'get'});
}