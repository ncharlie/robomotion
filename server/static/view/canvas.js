/* globals HTMLElement, window */
class Canvas extends HTMLElement {
    static observedAttributes = ['rid'];

    constructor() {
        super();
        const sheet = new CSSStyleSheet();
        sheet.replaceSync(/* css */ `
            #container {
                min-width: 600px;
                min-height: 600px;
                width: 45vw;
                height: 45vw;
                max-width: 100%;
                max-height: 100%;
                position: relative;
            }
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
            
            #robota {
                background-color: blue;
                bottom: 0px;
                left: 0px;
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

        // this.evtSource.close();
    }

    attributeChangedCallback(attr, oldValue, newValue) {
        console.log('map attribute changed', attr, oldValue, '->', newValue);
        if (attr === 'rid') {
            this.render();
        }
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
        <div id="container">
            <div class="robot" id="robota" hidden>
                <div class="robot-label">r1</div>
            </div>
        </div>
    `;
    }

    addListeners() {
        this.addEventListener('unauthorized', this.handleNoAuth);
    }

    removeListeners() {
        this.removeEventListener('unauthorized', this.handleNoAuth);
    }
}

window.customElements.define('map-module', Canvas);
