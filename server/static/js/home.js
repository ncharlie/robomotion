function dismissNotification(button) {
    // Get the parent notification element and remove it from the DOM
    const notification = button.parentElement;
    notification.remove();
}
