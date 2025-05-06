/* globals HTMLElement, window */
class Control extends HTMLElement {
    static observedAttributes = ['auth'];

    constructor() {
        super();
        const sheet = new CSSStyleSheet();
        sheet.replaceSync(/* css */ `
            .container {
                margin: 20px auto;
                font-family: Arial, sans-serif;
            }
            .form-group {
                display: flex;
                flex-direction: row;
                margin-bottom: 15px;
            }
            label {
                display: inline-block;
                padding: 5px;
                width: 100px;
                font-weight: bold;
                color: white;
            }
            input[type="text"] {
                padding: 5px;
            }
            span {
                display: inline-block;
                padding: 5px;
                width: 50px;
                color: white;
            }
            /* Control styling */
            .arrow-controls {
                margin-top: 20px;
                text-align: center;
            }
            .arrow-row {
                margin-bottom: 10px;
            }
            .arrow-row button {
                padding: 10px 15px;
                font-size: 16px;
                margin: 0 5px;
                cursor: pointer;
            }
            /* Notifications container */
            .notifications-container {
                margin-top: 20px;
                max-height: 400px;
                overflow-y: auto;
            }
            .notification {
                padding: 15px;
                border: 1px solid #ccc;
                background-color: #f9f9f9;
                display: flex;
                align-items: flex-start;
                margin-bottom: 10px;
                position: relative;
            }
            .alert-logo {
                width: 40px;
                height: 40px;
                background-color: #ffcc00;
                display: flex;
                align-items: center;
                justify-content: center;
                margin-right: 15px;
                font-weight: bold;
                color: #fff;
                border-radius: 50%;
            }
            .notification-content {
                flex: 1;
            }
            .notification-title {
                font-weight: bold;
                margin-bottom: 5px;
            }
            .notification-timestamp {
                font-size: 0.9em;
                color: #666;
                margin-bottom: 10px;
            }
            .notification-message {
                font-size: 1em;
            }
            .dismiss-button {
                background: transparent;
                border: none;
                position: absolute;
                top: 5px;
                right: 5px;
                font-size: 16px;
                cursor: pointer;
            }
      `);
        this.attachShadow({ mode: 'open' });
        this.shadowRoot.adoptedStyleSheets = [sheet];
        this.template = document.createElement('div');
    }

    connectedCallback() {
        console.log('control connected');
        this.shadowRoot.appendChild(this.template);
        this.render();
        this.addListeners();
    }

    attributeChangedCallback(attr, oldValue, newValue) {
        console.log(
            'control attribute changed',
            attr,
            oldValue,
            '->',
            newValue
        );
    }

    adoptedCallback() {
        console.log('control adopted');
        this.render();
    }

    disconnectedCallback() {
        console.log('control disconnected');
        this.removeListeners();
        this.shadowRoot.removeChild(this.template);
    }

    render() {
        this.template.innerHTML = /* html */ `
        <div class="container">
            <!-- Status Section -->
            <div class="form-group">
                <label for="robot-id">Robot ID:</label>
                <input type="text" id="robot-id" placeholder="Please select a robot" readonly>
            </div>
            <div class="form-group">
                <label for="x">X:</label>
                <input type="number" id="x" placeholder="Please select a robot" readonly>
            </div>
            <div class="form-group">
                <label for="y">Y:</label>
                <input type="number" id="y" placeholder="Please select a robot" readonly>
            </div>
            <div class="form-group">
                <label for="speed">Speed:</label>
                <input type="number" id="speed" placeholder="Please select a robot" readonly>
                <span>rpm</span>
            </div>
            <div class="form-group">
                <label for="heading">Heading:</label>
                <input type="number" id="heading" placeholder="Please select a robot" readonly>
                <span>degree</span>
            </div>
            <!-- Control Section -->
            <div class="arrow-controls" hidden>
                <div class="arrow-row">
                    <button id="forward">↑ Forward</button>
                </div>
                <div class="arrow-row">
                    <button id="leftward">← Left</button>
                    <button id="stop">■ Stop</button>
                    <button id="rightward">→ Right</button>
                </div>
                <div class="arrow-row">
                    <button id="backward">↓ Backward</button>
                </div>
            </div>
            <!-- Notifications Section -->
            <div class="notifications-container" hidden>
                <template>
                    <div class="notification">
                        <div class="alert-logo">!</div>
                        <div class="notification-content">
                            <div class="notification-title">Notification Title 1</div>
                            <div class="notification-timestamp">2025-05-06 12:34:56</div>
                            <div class="notification-message">This is the first notification message.</div>
                        </div>
                        <button class="dismiss-button" onclick="dismissNotification(this)">✖</button>
                    </div>
                </template>
            </div>
        </div>
        </div>
    `;

        if (this.hasAttribute('active')) {
            const elem = this.template.querySelector(
                `#${this.getAttribute('active')}`
            );
            if (elem) {
                elem.classList.add('active');
                elem.firstChild.href = '#';
            }
        }
    }

    fetchData() {
        // Fetch data from the server
        fetch('/api/control')
            .then((response) => response.json())
            .then((data) => {
                // Update the UI with the fetched data
                this.shadowRoot.querySelector('#robot-id').value = data.robotId;
                this.shadowRoot.querySelector('#x').value = data.x;
                this.shadowRoot.querySelector('#y').value = data.y;
                this.shadowRoot.querySelector('#speed').value = data.speed;
                this.shadowRoot.querySelector('#heading').value = data.heading;
            })
            .catch((error) => console.error('Error fetching data:', error));
    }

    handleNoAuth(e) {
        console.log('No Auth');
        const loginModule = this.shadowRoot.querySelector('login-module');
        loginModule.removeAttribute('name');
    }

    addListeners() {
        this.addEventListener('unauthorized', this.handleNoAuth);
    }

    removeListeners() {
        this.removeEventListener('unauthorized', this.handleNoAuth);
    }
}

window.customElements.define('control-module', Control);
