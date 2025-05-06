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

function move(direction) {
    fetch('/move', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            robotId: 'r1',
            direction: direction
        })
    }).then((response) => {
        if (response.ok) {
            // console.log(`Moved ${direction}`);
        } else {
            console.error('Error moving robot:', response.statusText);
        }
    });
}

function addNotification(e) {
    const data = JSON.parse(e.data);

    const controlRoot =
        document.getElementsByTagName('control-module')[0].shadowRoot;
    const template = controlRoot.querySelector('#notification-template');

    const clone = template.content.cloneNode(true);
    // Update notification content
    clone.querySelector('.notification-timestamp').textContent = data.ts;
    clone.querySelector('.notification-message').textContent =
        'Robot ' + data.r + ' encountered an obstacle';

    // Append to the notifications container
    const container = controlRoot.querySelector('.notifications-container');
    container.appendChild(clone);
}

function startNoti() {
    const evtSource = new EventSource('/noti?id=r1', {
        withCredentials: true
    });
    evtSource.addEventListener('Notification', addNotification);
}
