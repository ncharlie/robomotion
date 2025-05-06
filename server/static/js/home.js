function dismissNotification(button) {
    // Get the parent notification element and remove it from the DOM
    const notification = button.parentElement;
    notification.remove();
}

function locationUpdate(e) {
    const data = JSON.parse(e.data);
    // console.log('Location', data);

    const mapRoot = document.getElementsByTagName('map-module')[0].shadowRoot;
    const container = mapRoot.getElementById('container');
    const marker = mapRoot.getElementById('robota');

    // Scale the coordinate from [0,200] to [0, container width/height]
    const computedX = (data.x / 200) * container.clientWidth;
    const computedY = (data.y / 200) * container.clientHeight;

    // Center the marker at (x, y)
    marker.style.left = computedX - marker.offsetWidth / 2 + 'px';
    marker.style.bottom = computedY - marker.offsetHeight / 2 + 'px';
    marker.hidden = false;

    const controlRoot =
        document.getElementsByTagName('control-module')[0].shadowRoot;
    controlRoot.querySelector('#robot-id').value = data.r;
    controlRoot.querySelector('#x').value = data.x;
    controlRoot.querySelector('#y').value = data.y;
    controlRoot.querySelector('#speed').value = data.s;
    controlRoot.querySelector('#heading').value = data.h;
}

function startUpdate() {
    const evtSource = new EventSource('/update?id=r1', {
        withCredentials: true
    });
    evtSource.addEventListener('Location', locationUpdate);
}
