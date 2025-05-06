/* globals HTMLElement, window */
class Canvas extends HTMLElement {
    static observedAttributes = ['auth'];

    constructor() {
        super();
        const sheet = new CSSStyleSheet();
        sheet.replaceSync(/* css */ `
          .robot-label {
            position: absolute;
            top: -20px;
            left: 50%;
            transform: translateX(-50%);
            font-size: 12px;
            font-weight: bold;
            color: white;
            background: black;
            padding: 2px 5px;
            border-radius: 3px;
        }
        .robot {
            width: 20px;
            height: 20px;
            border-radius: 50%;
            position: absolute;
            transition: top 0.3s, left 0.3s;
        }
        
        #robot1 {
            background-color: blue;
            top: 200px;
            left: 200px;
        }
        
        #robot2 {
            background-color: red;
            top: 63%;
            left: 81%;
        }
        
        #robot3 {
            background-color: green;
            top: 475px;
            left: 500px;
        }
        
        #robot4 {
            background-color: yellow;
            top: 475px;
            left: 500px;
        }
      `);
        this.attachShadow({ mode: 'open' });
        this.shadowRoot.adoptedStyleSheets = [sheet];
        this.template = document.createElement('div');
    }

    connectedCallback() {
        console.log('map connected');
        // console.log(this.style);
        // this.shadowRoot.appendChild(this.style);
        this.shadowRoot.appendChild(this.template);
        this.render();
        this.addListeners();
    }

    attributeChangedCallback(attr, oldValue, newValue) {
        console.log('map attribute changed', attr, oldValue, '->', newValue);
    }

    adoptedCallback() {
        console.log('map adopted');
        this.render();
    }

    disconnectedCallback() {
        console.log('map disconnected');
        this.removeListeners();
        this.shadowRoot.removeChild(this.template);
    }

    render() {
        this.template.innerHTML = /* html */ `
        <nav>
            <ul>
                <li id="home"><a href="index">Home</a></li>
                <li id="map"><a href="map">Dashboard</a></li>
                <li id="history"><a href="history">History</a></li>
            </ul>
        </nav>
        <slot name="body"></slot>
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

window.customElements.define('map-module', Canvas);
