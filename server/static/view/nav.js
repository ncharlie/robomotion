/* globals HTMLElement, window */
class Nav extends HTMLElement {
  constructor() {
    super();
    this.current = "login-module";
    this.attachShadow({ mode: "open" });
    this.template = document.createElement("div");
    this.style = document.createElement("style");
    this.style.textContent = /* css */ `
        nav {
            background-color: #333;
            overflow: hidden;
            font-family: Verdana,sans-serif;
            line-height: 1.5;
            position: fixed;
            top: 0;
            width: 100%;
        }
        nav ul {
            list-style-type: none;
            margin: 0;
            padding: 0;
            overflow: hidden;
        }

        nav li {
            float: left;
        }

        nav li a {
            display: block;
            color: white;
            text-align: center;
            padding: 14px 16px;
            text-decoration: none;
            min-width: 50px;
            height: 100;
        }

        nav li:last-child{
            float: right;
        }
        nav li:hover:not(.active) a{
            background-color: #111;
        }

        nav li.active {
            background-color: #04AA6D;
        }
    `;
  }

  connectedCallback() {
    console.log("nav connected");
    // console.log(this.style);
    this.shadowRoot.appendChild(this.style);
    this.shadowRoot.appendChild(this.template);
    this.render();
    this.addListeners();
  }

  attributeChangedCallback(attr, oldValue, newValue) {
    console.log("nav attribute changed", attr, oldValue, "->", newValue);
  }

  adoptedCallback() {
    console.log("nav adopted");
    this.render();
  }

  disconnectedCallback() {
    console.log("nav disconnected");
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

    if (this.hasAttribute("active")) {
      const elem = this.template.querySelector(
        `#${this.getAttribute("active")}`
      );
      if (elem) {
        elem.classList.add("active");
        elem.firstChild.href = "#";
      }
    }
  }

  handleNoAuth(e) {
    console.log("No Auth");
    const loginModule = this.shadowRoot.querySelector("login-module");
    loginModule.removeAttribute("name");
  }

  addListeners() {
    this.addEventListener("unauthorized", this.handleNoAuth);
  }

  removeListeners() {
    this.removeEventListener("unauthorized", this.handleNoAuth);
  }
}

window.customElements.define("nav-module", Nav);
